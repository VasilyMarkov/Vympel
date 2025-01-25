#include <iostream>
#include <algorithm>
#include <memory>
#include <QCoreApplication>
#include "app.hpp"

int main(int argc, char *argv[])
{
  try {

    QCoreApplication app(argc, argv);

    app::Application application(app);

    return app.exec();
  }

  catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
}
