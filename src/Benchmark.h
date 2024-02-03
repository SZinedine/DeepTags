#ifndef BENCHMARK__H
#define BENCHMARK__H

#ifdef BENCHMARKS
    #include <chrono>
    #include <string>

class Benchmark final {
    using time_point          = std::chrono::steady_clock::time_point;
    static constexpr auto now = std::chrono::steady_clock::now;

public:
    explicit Benchmark(std::string message) : mMessage(std::move(message)) { begin(); }

    void begin() { mBegin = now(); }

    void end() {
        if (!mEnded) {
            mEnd   = now();
            auto t = std::chrono::duration_cast<std::chrono::milliseconds>(mEnd - mBegin).count();
            std::printf("%s: %ld[ms]\n", mMessage.c_str(), t);
        }

        mEnded = true;
    }

    ~Benchmark() { end(); }

private:
    std::string mMessage;
    bool mEnded{ false };
    time_point mBegin;
    time_point mEnd;
};

#else

struct Benchmark {
    explicit Benchmark([[maybe_unused]] const char* message) {}
    void start() {}
    void end() {}
};

#endif

#endif   // BENCHMARK__H
