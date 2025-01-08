#include "thread/thread_pool.hpp"
#include "camera/film.hpp"
#include <iostream>
#include <mutex>
#include <glm/glm.hpp>

std::mutex main_mutex;
class SimpleTask : public Task {
  public:
    SimpleTask(int id) : job_id{id} {}
    void run() override {
        std::unique_lock<std::mutex> lock(main_mutex);
        std::cout << "Thread working on job " << job_id << std::endl;
    }

  private:
    int job_id;
};

int main() {

    // ThreadPool pool{};
    // for (int i = 0; i < 100; i++) {
    //     pool.addTask(new SimpleTask(i));
    // }
    Film film{800, 600};
    for(int x=0;x<film.getWidth();x++)
    {
      for(int y=0;y<film.getHeight();y++){
        film.setPixel(x,y,glm::vec3(1.0f,0.0f,0.0f));
      }
    }

    return 0;
}