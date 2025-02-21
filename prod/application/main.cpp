#include <iostream>
#include <algorithm>
#include <memory>
#include <QCoreApplication>
#include "app.hpp"

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  app::Application application(app);
  
  return app.exec();
}
