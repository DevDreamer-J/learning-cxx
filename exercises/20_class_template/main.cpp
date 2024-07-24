#include "../exercise.h"
#include <cstring>
// READ: 类模板 <https://zh.cppreference.com/w/cpp/language/class_template>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;

    Tensor4D(unsigned int const shape_[4], T const *data_) {
        // TODO: 填入正确的 shape 并计算 size
        unsigned int size = 1;
        for (int i = 0; i < 4; i++) {
            size = size * shape_[i];
            shape[i] = shape_[i];
        }
        data = new T[size];
        memcpy(data, data_, size * sizeof(T));
    }
    ~Tensor4D() {
        delete[] data;
    }

    // 为了保持简单，禁止复制和移动
    Tensor4D(Tensor4D const &) = delete;
    Tensor4D(Tensor4D &&) noexcept = delete;

    // 这个加法需要支持“单向广播”。
    // 具体来说，`others` 可以具有与 `this` 不同的形状，形状不同的维度长度必须为 1。
    // `others` 长度为 1 但 `this` 长度不为 1 的维度将发生广播计算。
    // 例如，`this` 形状为 `[1, 2, 3, 4]`，`others` 形状为 `[1, 2, 1, 4]`，
    // 则 `this` 与 `others` 相加时，3 个形状为 `[1, 2, 1, 4]` 的子张量各自与 `others` 对应项相加。

    Tensor4D &operator+=(Tensor4D const &others) {
        // TODO: 实现单向广播的加法
        unsigned int weidu_this[3] = {
            this->shape[1] * this->shape[2] * this->shape[3],
            this->shape[2] * this->shape[3],
            this->shape[3]};
        unsigned int weidu_others[3] = {
            others.shape[1] * others.shape[2] * others.shape[3],
            others.shape[2] * others.shape[3],
            others.shape[3]};
        for (unsigned int i = 0; i < this->shape[0] * this->shape[1] * this->shape[2] * this->shape[3]; i++) {
            unsigned int p_this[3] = {0};
            unsigned int p_others[3] = {0};
            {
                int k = i;
                p_this[0] = k / weidu_this[1];
                k = k % weidu_this[1];
                p_this[1] = k / weidu_this[2];
                k = k % weidu_this[2];
                p_this[2] = k;
            }
            for (int j = 0; j < 3; j++) {
                if (p_this[j] < weidu_others[j]) {
                    p_others[j] = p_this[j];
                } else {
                    p_others[j] = weidu_others[j]-1;
                }
            }
            this->data[i] = this->data[i] + others.data[p_others[0] * weidu_others[1] + p_others[1] * weidu_others[2] + p_others[2]];
        }
        return *this;
    };
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D(shape, data);
        auto t1 = Tensor4D(shape, data);
        t0 += t1;
        for (unsigned int i = 0; i < sizeof(data) / sizeof(int); i++) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (unsigned int i = 0; i < sizeof(d0) / sizeof(int); i++) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (unsigned int i = 0; i < sizeof(d0) / sizeof(double); i++) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}