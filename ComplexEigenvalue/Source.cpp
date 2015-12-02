/*
���f�G�l���M�[�ŗL�l�̒T��
 -> �����Ƌ������p�����[�^�ɂ���3�����v���b�g
*/

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "libfftw3-3.lib")
#include<iostream>
#include<iomanip>
#include<fstream>
#include<cmath>
#include<vector>
#include<utility>
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

const double X_BEGIN = -5.0, X_END = 25.0; //�n�̗��[
const double L = X_END - X_BEGIN; //��ԕ�
const int N = 256; //��ԕ�����
const double DELTA_X = L / N;

const double T_END = 50; //�I������
const int TN = 500; //���ԕ�����
const double dt = T_END / TN; //���ԍ��ݕ�

const double S = 0.9; //�g���̕�
const double X = -0.5; //�����g���̌��_����̂���
const double X_OPT = 4.0; //���w�|�e���V������������ʒu
const double b = 1.0 / 3.0; //3���|�e���V�����̌W��

//const double E_BEGIN = -1.5, E_END = 1.0; //�T������G�l���M�[�̗��[
//const int EN = 1000; //�G�l���M�[������
//const double dE = (E_END - E_BEGIN) / EN; //�G�l���M�[���ݕ�

const double E_BEGIN_real = -1.4, E_END_real = 0.0;
const int EN_real = 400;
const double dE_real = (E_END_real - E_BEGIN_real) / EN_real;

const double E_BEGIN_imag = -0.02, E_END_imag = 0.02;
const int EN_imag = 100;
const double dE_imag = (E_END_imag - E_BEGIN_imag) / EN_imag;

const double E_BEGIN = E_BEGIN_real, E_END = E_END_real; //�T������G�l���M�[�̗��[
const int EN = EN_real; //�G�l���M�[������
const double dE = (E_END - E_BEGIN) / EN; //�G�l���M�[���ݕ�

double i2x(int i){
    return X_BEGIN + (i + 1) * DELTA_X;
}
double i2k(int i){
    return 2 * M_PI * (i < N / 2 ? i : i - N) / L;
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

//�������p�֐��̒�`
void init(vC &f){
    for (int i = 0; i < N; i++){
        f[i] = firstExcited(i2x(i), X);
    }
}

//�|�e���V�����֐��̒�`
double V(double x){
    return (1.0 / 2.0) * x*x - (b / 3.0) * x*x*x - 1.0 / (6 * b * b);
    //return (1.0 / 2.0) * x*x;
}

//���w�|�e���V�����̒�`
double V_opt(double x){
    return x > X_OPT ? x*x : 0.0;
}

//Simpson�ϕ�
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

//dt�������Ԕ��W������֐�
void timeEvolution(vC &f, fftw_plan plan_for, fftw_plan plan_back){
    //�|�e���V���������̌v�Z
    for (int j = 0; j < N; j++){
        f[j] *= polar(1.0 / N, -V(i2x(j)) * dt); // 1/N�͐��K�����q
    }

    //���w�|�e���V���������̌v�Z
    for (int j = 0; j < N; j++){
        f[j] *= exp(-V_opt(i2x(j)) * dt);
    }

    //f(x, t) -> g(k, t)
    fftw_execute(plan_for);

    //���Ԕ��W�̌v�Z
    for (int j = 0; j < N; j++){
        f[j] *= polar(1.0, -i2k(j) * i2k(j) * dt / 2.0);
    }

    //f(k, t+dt) -> f(x, t+dt)
    fftw_execute(plan_back);
}

//�G�l���M�[�s�[�N�̃C���f�b�N�X�����߂�֐�
void getPeaks(vector<pair<double, int>> &peak, vector<double> &res){
    //�����l�������畉�ɕς�����Ƃ���̒l�ƃC���f�b�N�X
    for (int i = 1; i < EN - 1; i++){
        if (res[i - 1] < res[i] && res[i] > res[i + 1]){
            peak.push_back(make_pair(res[i], i));
        }
    }

    //�s�[�N�l�̑傫�����Ƀ\�[�g
    sort(peak.begin(), peak.end(), [](const pair<double, int> &i, const pair<double, int> &j){ return i.first > j.first; });

    double E_th = 0.02; //�������l
    //�������l�ȉ��̗v�f���폜
    peak.erase(remove_if(peak.begin(), peak.end(), [E_th](pair<double, int> pair) {return pair.first < E_th; }), peak.end());

    //����ꂽ�s�[�N�l��\��
    cout << "index" << "\t" << "E" << "\t" << "peak value" << endl;
    cout << setprecision(3);
    for (auto pair : peak){
        cout << pair.second << "\t" << i2E(E_BEGIN, pair.second, dE);
        cout << "\t" << pair.first << endl;
    }
}

//�ŗL��Ԃ̒��o
void getEigenfunction(vC &phi, vC &f, fftw_plan plan_for, fftw_plan plan_back, double energy){
    for (int i = 0; i <= TN; i++){
        //�ϕ��v�Z
        for (int j = 0; j < N; j++){
            phi[j] += f[j] * polar(dt, energy * (i * dt));
        }

        timeEvolution(f, plan_for, plan_back);
    }

    for (auto &val : phi){
        val /= T_END;
    }
}

int main(){
    auto start = system_clock::now();
    vC f(N);
    vvC A(EN, vC(N));
    vvvC C(EN_real, vvC(EN_imag, vC(N)));

    //������Fourier�ϊ�
    fftw_plan plan_for = fftw_plan_dft_1d(N, fftwcast(f.data()), fftwcast(f.data()), FFTW_FORWARD, FFTW_MEASURE);

    //�t����
    fftw_plan plan_back = fftw_plan_dft_1d(N, fftwcast(f.data()), fftwcast(f.data()), FFTW_BACKWARD, FFTW_MEASURE);

    init(f); //��������f(x,0)�̐ݒ�

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

        //�ϕ��v�Z
        for (int j = 0; j < EN; j++){
            for (int k = 0; k < N; k++){
                A[j][k] += f[k] * polar(dt, i2E(E_BEGIN, j, dE) * (i * dt));
            }
        }

        for (int j = 0; j < EN_real; j++){
            for (int k = 0; k < EN_imag; k++){
                for (int l = 0; l < N; l++){
                    C[j][k][l] += f[k] * polar(dt, i2E(E_BEGIN_real, j, dE_real) * (i * dt)) * exp(i2E(E_BEGIN_imag, k, dE_imag) * (i * dt));
                }
            }
        }

        //���Ԕ��W
        timeEvolution(f, plan_for, plan_back);
    }

    for (auto &vec : A){
        for (auto &val : vec){
            val /= T_END;
        }
    }

    for (int i = 0; i < EN_real; i++){
        for (int j = 0; j < EN_imag; j++){
            for (int k = 0; k < N; k++){
                C[i][j][k] *= exp(-i2E(E_BEGIN_imag, j, dE_imag) * T_END) / T_END;
            }
        }
    }

    ofs.open("./output/energy.txt");
    if (!ofs){
        cerr << "file open error!" << endl;
        exit(1);
    }

    vector<double> res(EN); //���ʊi�[�p�z��
    ofs << scientific;
    for (int i = 0; i < EN; i++){
        res[i] = simpson(A[i]);
        ofs << i2E(E_BEGIN, i, dE) << "\t" << res[i] << endl;
    }

    ofs.close();

    ofs.open("./output/energy_complex.txt");
    if (!ofs){
        cerr << "file open error!" << endl;
        exit(1);
    }

    vector<vector<double>> res_complex(EN_real, vector<double>(EN_imag));
    ofs << scientific;
    for (int i = 0; i < EN_real; i++){
        for (int j = 0; j < EN_imag; j++){
            res_complex[i][j] = simpson(C[i][j]);
            ofs << i2E(E_BEGIN_real, i, dE_real) << "\t";
            ofs << i2E(E_BEGIN_imag, j, dE_imag) << "\t";
            ofs << res_complex[i][j] << endl;
        }
        ofs << endl;
    }

    vector<pair<double, int>> peak; //�s�[�N�l�ƃC���f�b�N�X���i�[����pair
    getPeaks(peak, res);

    auto end = system_clock::now();
    auto dur = end - start;
    auto sec = duration_cast<seconds>(dur).count();
    auto min = duration_cast<minutes>(dur).count();

    cout << endl;
    cout << "execution time : " << sec << "sec" << " ";
    cout << '(' << min << "min)" << endl;
    cout << endl;

    return 0;
}