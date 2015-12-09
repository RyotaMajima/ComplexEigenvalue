/*
複素エネルギー固有値の探索
 -> 実部と虚部をパラメータにして3次元プロット
*/

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "libfftw3-3.lib")
#include<iostream>
#include<iomanip>
#include<fstream>
#include<cstdio>
#include<cmath>
#include<vector>
#include<utility>
#include<string>
#include<algorithm>
#include<functional>
#include<complex>
#include<chrono>
#include<fftw3.h>
using namespace std;
using namespace std::chrono;
using Complex = complex<double>;
using vC = vector<Complex>;
using vvC = vector<vC>;
using vvvC = vector<vvC>;
inline fftw_complex* fftwcast(Complex* f){ return reinterpret_cast<fftw_complex*>(f); }

const double X_BEGIN = -5.0, X_END = 25.0; //系の両端
const double L = X_END - X_BEGIN; //空間幅
const int N = 256; //空間分割数
const double DELTA_X = L / N;

const double T_END = 10; //終了時刻
const int TN = T_END * 2; //時間分割数
const double dt = T_END / TN; //時間刻み幅

const double S = 0.9; //波束の幅
const double X = -0.5; //初期波束の原点からのずれ
const double X_OPT = 4.0; //光学ポテンシャルをかける位置
const double b = 1.0 / 3.0; //3次ポテンシャルの係数

const double E_BEGIN_real = -1.1, E_END_real = -1.0;
const int EN_real = 50;
const double dE_real = (E_END_real - E_BEGIN_real) / EN_real;

const double E_BEGIN_imag = -0.5, E_END_imag = 0.5;
const int EN_imag = 200;
const double dE_imag = (E_END_imag - E_BEGIN_imag) / EN_imag;

double i2x(int i){
    return X_BEGIN + i * DELTA_X;
}
double i2k(int i){
    return 2 * M_PI * ((i < N / 2)? i : i - N) / L;
}
double i2E(double begin, int index, double width){
    return begin + index * width;
}

double groundState(double x, double X){
    return pow(1 / (S*S * M_PI), 1.0 / 4.0) * exp(-(x - X) * (x - X) / (2 * S*S));
}
double firstExcited(double x, double X){
    return ((sqrt(2.0) * (x - X)) / S) * pow(1 / (S*S * M_PI), 1.0 / 4.0) * exp(-(x - X) * (x - X) / (2 * S*S));
}
double secondExcited(double x, double X){
    return sqrt(1 / (3 * S*S*S*S - 2 * S*S + 1)) * pow(1 / (S*S * M_PI), 1.0 / 4.0) * (2 * (x - X) * (x - X) - 1) * exp(-(x - X) * (x - X) / (2 * S*S));
}

//初期化用関数の定義
void init(vC &f){
    for (int i = 0; i < N; i++){
        f[i] = firstExcited(i2x(i), X);
    }
}

//ポテンシャル関数の定義
double V(double x){
    return (1.0 / 2.0) * x*x - (b / 3.0) * x*x*x - 1.0 / (6 * b * b);
    //return (1.0 / 2.0) * x*x;
}

//光学ポテンシャルの定義
double V_opt(double x){
    return x > X_OPT ? x*x : 0.0;
}

//Simpson積分
double simpson(vC &f){
    double S_even, S_odd;

    S_even = S_odd = 0.0;

    for (int i = 1; i < (N / 2) - 1; i++){
        S_even += norm(f[2 * i]);
    }

    for (int i = 1; i < N / 2; i++){
        S_odd += norm(f[2 * i - 1]);
    }

    return DELTA_X * (norm(f[0]) + 2 * S_even + 4 * S_odd + norm(f[N - 1])) / 3.0;
}

//dtだけ時間発展させる関数
void timeEvolution(vC &f, fftw_plan plan_for, fftw_plan plan_back){
    //ポテンシャル部分の計算
    for (int j = 0; j < N; j++){
        f[j] *= polar(1.0 / N, -V(i2x(j)) * dt); // 1/Nは正規化因子
    }

    //光学ポテンシャル部分の計算
    for (int j = 0; j < N; j++){
        f[j] *= exp(-V_opt(i2x(j)) * dt);
    }

    //f(x, t) -> g(k, t)
    fftw_execute(plan_for);

    //時間発展の計算
    for (int j = 0; j < N; j++){
        f[j] *= polar(1.0, -i2k(j) * i2k(j) * dt / 2.0);
    }

    //f(k, t+dt) -> f(x, t+dt)
    fftw_execute(plan_back);
}

//複素エネルギーピークのインデックスを求める関数
void getComplexPeaks(vector<tuple<double, int, int>> &peak, vector<vector<double>> &res){
    //微分値が正から負に変わったところの値とインデックス
    for (int i = 1; i < EN_real; i++){
        for (int j = 1; j < EN_imag; j++){
            if (res[i - 1][j] < res[i][j] && res[i][j] > res[i + 1][j] && res[i][j - 1] < res[i][j] && res[i][j] > res[i][j + 1]){
                peak.push_back(make_tuple(res[i][j], i, j));
            }
        }
    }

    //ピーク値の大きい順にソート
    sort(peak.begin(), peak.end(), [](const tuple<double, int, int> &i, const tuple<double, int, int> &j){ return get<0>(i) > get<0>(j); });
    if (peak.empty()){
        cout << "no peak" << endl;
        exit(1);
    }
    else{
        double E_th = get<0>(peak[0]) / 10; //しきい値
        //しきい値以下の要素を削除
        peak.erase(remove_if(peak.begin(), peak.end(), [E_th](tuple<double, int, int> tuple) {return get<0>(tuple) < E_th; }), peak.end());

        //実部の小さい順にソート
        sort(peak.begin(), peak.end(), [](const tuple<double, int, int> &i, const tuple<double, int, int> &j){ return get<1>(i) < get<1>(j); });

        //得られたピーク値を表示
        cout << "---- complex ver. ----" << endl << endl;

        cout << "threshold value : " << E_th << endl;
        cout << "Re" << "\t" << "Im" << "\t" << "peak value" << endl;
        for (auto tuple : peak){
            double Re = i2E(E_BEGIN_real, get<1>(tuple), dE_real);
            double Im = i2E(E_BEGIN_imag, get<2>(tuple), dE_imag);
            printf("%.3lf\t%.3lf\t%.3lf\n", Re, Im, get<0>(tuple));
        }
    }
}

//固有状態の抽出
void getComplexEigenfunction(vC &phi, vC &f, fftw_plan plan_for, fftw_plan plan_back, double energy_real, double energy_imag){
    for (int i = 0; i <= TN; i++){
        //積分計算
        for (int j = 0; j < N; j++){
            phi[j] += f[j] * polar(dt, energy_real * (i * dt)) * exp(-energy_imag * (i * dt));
        }

        timeEvolution(f, plan_for, plan_back);
    }

    for (auto &val : phi){
        val *= exp(-fabs(energy_imag) * T_END) / T_END;
    }
}

int main(){
    auto start = system_clock::now();
    vC f(N);
    vvvC C(EN_real + 1, vvC(EN_imag + 1, vC(N)));

    //順方向Fourier変換
    fftw_plan plan_for = fftw_plan_dft_1d(N, fftwcast(f.data()), fftwcast(f.data()), FFTW_FORWARD, FFTW_MEASURE);

    //逆方向
    fftw_plan plan_back = fftw_plan_dft_1d(N, fftwcast(f.data()), fftwcast(f.data()), FFTW_BACKWARD, FFTW_MEASURE);

    printf("---- calculation parameters ----\n");
    printf("system size : \t [%.1f, %.1f]\n", X_BEGIN, X_END);
    printf("partition number : N = %d\n", N);
    printf("calculation time : T = %.0f\n", T_END);
    printf("time step : dt = %.1f\n", dt);
    printf("energy width (real) : [%.2f, %.2f] N = %d\n", E_BEGIN_real, E_END_real, EN_real);
    printf("energy width (imag) : [%.2f, %.2f] N = %d\n", E_BEGIN_imag, E_END_imag, EN_imag);
    printf("--------------------------------\n");

    init(f); //初期条件f(x,0)の設定

    ofstream ofs;
    char filename[50];

    for (int i = 0; i <= TN; i++){
        sprintf(filename, "./output/timeEvo/output%03d.txt", i);
        ofs.open(filename);
        if (!ofs){
            cerr << "file open error!" << endl;
            exit(1);
        }

        for (int j = 0; j < N; j++){
            ofs << i2x(j) << "\t" << norm(f[j]) << "\t" << V(i2x(j)) << endl;
        }

        ofs.close();

        for (int j = 0; j <= EN_real; j++){
            for (int k = 0; k <= EN_imag; k++){
                for (int l = 0; l < N; l++){
                    C[j][k][l] += f[l] * polar(dt, i2E(E_BEGIN_real, j, dE_real) * (i * dt)) * exp(-i2E(E_BEGIN_imag, k, dE_imag) * (i * dt));
                }
            }
        }

        //時間発展
        timeEvolution(f, plan_for, plan_back);
    }

    for (int i = 0; i <= EN_real; i++){
        for (int j = 0; j <= EN_imag; j++){
            for (int k = 0; k < N; k++){
                C[i][j][k] *= exp(-fabs(i2E(E_BEGIN_imag, j, dE_imag)) * T_END) / T_END;
            }
        }
    }

    string str = "./output/energy_complex_T_" + to_string((int)T_END) + ".txt";
    ofs.open(str);
    if (!ofs){
        cerr << "file open error!" << endl;
        exit(1);
    }

    vector<vector<double>> res_complex(EN_real + 1, vector<double>(EN_imag + 1));
    ofs << scientific;
    for (int i = 0; i <= EN_real; i++){
        for (int j = 0; j <= EN_imag; j++){
            res_complex[i][j] = simpson(C[i][j]);
            ofs << i2E(E_BEGIN_real, i, dE_real) << "\t";
            ofs << i2E(E_BEGIN_imag, j, dE_imag) << "\t";
            ofs << res_complex[i][j] << endl;
        }
        ofs << endl;
    }

    ofs.close();;

    vector<tuple<double, int, int>> peak_complex; //ピーク値と実部・虚部のインデックスを格納するtuple

    getComplexPeaks(peak_complex, res_complex); //固有値のピークの探索

    int peakNum = peak_complex.size();
    vvC phi(peakNum, vC(N)); //固有状態格納用配列

    //固有状態の抽出
    for (int i = 0; i < peakNum; i++){
        init(f);
        getComplexEigenfunction(phi[i], f, plan_for, plan_back, i2E(E_BEGIN_real, get<1>(peak_complex[i]), dE_real), i2E(E_BEGIN_imag, get<2>(peak_complex[i]), dE_imag));
    }

    //比較のため調和振動子の解を出力
    vector<vector<double>> ho(2, vector<double>(N));
    for (int i = 0; i < N; i++){
        double x = i2x(i);
        ho[0][i] = norm(groundState(x, 0.0));
        ho[1][i] = norm(firstExcited(x, 0.0));
    }

    ofs.open("./output/harmonic.txt");
    if (!ofs){
        cerr << "file open error!" << endl;
        exit(1);
    }

    for (int i = 0; i < N; i++){
        ofs << i2x(i) << "\t";
        for (int j = 0; j < 2; j++){
            ofs << ho[j][i] << "\t";
        }
        ofs << endl;
    }

    ofs.close();

    //再規格化
    for (int i = 0; i < peakNum; i++){
        double sNorm = simpson(phi[i]);
        for (int j = 0; j < N; j++){
            phi[i][j] = norm(phi[i][j]) / sNorm;
        }
    }

    str = "./output/phi_" + to_string((int)T_END) + ".txt";
    ofs.open(str);
    if (!ofs){
        cerr << "file open error!" << endl;
        exit(1);
    }

    //ファイル書き込み
    for (int i = 0; i < N; i++){
        ofs << i2x(i) << "\t" << V(i2x(i)) << "\t";
        for (int j = 0; j < peakNum; j++) {
            ofs << real(phi[j][i]) << "\t";
        }
        ofs << endl;
    }

    ofs.close();

    fftw_destroy_plan(plan_for);
    fftw_destroy_plan(plan_back);

    auto end = system_clock::now();
    auto dur = end - start;
    auto sec = duration_cast<seconds>(dur).count();
    auto min = duration_cast<minutes>(dur).count();

    cout << endl;
    cout << "execution time : " << sec << "sec" << " ";
    cout << "(" << min << "min)" << endl;
    cout << endl;
    
    return 0;
}