#include <iostream>
#include <random>

// feature flag is being set by compiler when comiling with -march=armv8-a+sve
#ifdef __ARM_FEATURE_SVE
#include <arm_sve.h>
#endif

void printvec(std::string name, std::vector<float32_t> &v) {
    std::cout << name << ":" << std::endl;
    for (auto elem : v) {
        std::cout << elem << std::endl;
    }
}

void show_pr(svbool_t tp) {
    int n = svcntb();
    std::vector<int8_t> a(n);
    std::vector<int8_t> b(n);
    std::fill(a.begin(), a.end(), 1);
    std::fill(b.begin(), b.end(), 0);
    svint8_t va = svld1_s8(tp, a.data());
    svst1_s8(tp, b.data(), va);
    for (int i = 0; i < n; i++) {
        std::cout << (int)b[n - i - 1];
    }
    std::cout << std::endl;
}

int main (void) {

    std::cout << "Hello Arm!" << std::endl;

    int num_samples = 4*10;
    unsigned int num_iterations = 10000;

    // random generator
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<float32_t> d{-1, 1};

    std::vector<float32_t> a_vec(num_samples);
    std::vector<float32_t> b_vec(num_samples);
    std::vector<float32_t> c_vec(num_samples);

    // Initialize input vectors with random data
    // 'elem' is a reference
    for (auto &elem : a_vec) {
        elem = d(gen);
    }
    for (auto &elem : b_vec) {
        elem = d(gen);
    }
    printvec("a", a_vec);
    printvec("b", b_vec);
    
    // create a predication for "all elements"
    svbool_t pg = svptrue_b32();
    show_pr(pg);
    // number of 32-bit elements in registers
    int n = svcntw();

    while (num_iterations) {
        // create raw data pointers to vector content
        float32_t* a = a_vec.data();
        float32_t* b = b_vec.data();
        float32_t* c = c_vec.data();
        for (int i = 0; i < num_samples; i += n) {
            // load 'a' and 'b' to register and multiply. Store in 'c'.
            svfloat32_t a_reg = svld1_f32(pg, a);
            svfloat32_t b_reg = svld1_f32(pg, b);
            svfloat32_t c_reg = svmul_f32_m(pg, a_reg, b_reg);
            svst1_f32(pg, c, c_reg);
            a += n;
            b += n;
            c += n;
        }
        num_iterations--;
    }

    printvec("c", c_vec);
    return 0;
}


// TODO:s
// 1. Profile the interesting part
