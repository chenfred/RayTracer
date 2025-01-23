#include "renderer/renderer.hpp"
#include "util/progress_bar.hpp"
#include "util/timer.hpp"

void Renderer::render(size_t spp, const std::filesystem::path &savePath, ThreadPool *threadPool) {
    if (spp <= 1) {
        render(savePath, threadPool);
        return;
    }

    size_t spp_cur = 0, inc_spp = 1;
    auto &film = camera.getFilm();
    film.clear();

    Timer timer{std::format("Rendering {} SPPs", spp)};
    ProgressBar bar{"Multi-Sample Rendering"};

    auto f = [&](size_t x, size_t y) -> void {
        for (size_t i = 0; i < inc_spp; ++i) {
            film.addPixelSample(x, y, renderPixel(x, y));
        }
    };
    while (spp_cur < spp) {
        if (threadPool) {
            threadPool->parallelFor(film.getWidth(), film.getHeight(), f);
            threadPool->wait();
        } else {
            for (size_t x = 0; x < film.getWidth(); ++x) {
                for (size_t y = 0; y < film.getHeight(); ++y) {
                    f(x, y);
                }
            }
        }

        bar.update(static_cast<double>(spp_cur) / spp);
        film.save(savePath, threadPool);

        spp_cur += inc_spp;
        inc_spp = std::min<size_t>(spp_cur, 32);
    }
    bar.done();
    timer.conclude();
}

void Renderer::render(const std::filesystem::path &savePath, ThreadPool *threadPool) {
    auto &film = camera.getFilm();
    film.clear();
    size_t num_pixels_total = film.getWidth() * film.getHeight();
    size_t rendering_count = 0;

    Timer timer{"Rendering 1 SPP"};
    ProgressBar bar{"Rendering"};
    auto f = [&](size_t x, size_t y) -> void {
        auto num_pixels_finished = ++rendering_count;
        film.addPixelSample(x, y, renderPixel(x, y));
        if (num_pixels_finished % film.getWidth() == 0) {
            bar.update(static_cast<double>(num_pixels_finished) / num_pixels_total);
        }
    };
    if (threadPool) {
        threadPool->parallelFor(film.getWidth(), film.getHeight(), f);
        threadPool->wait();
    } else {
        for (size_t x = 0; x < film.getWidth(); ++x) {
            for (size_t y = 0; y < film.getHeight(); ++y) {
                f(x, y);
            }
        }
    }
    bar.done();
    timer.conclude();

    film.save(savePath, threadPool);
}