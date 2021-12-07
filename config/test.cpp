#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main () {
  string myText;
  ifstream cnfg_file("filename.txt");

  while (getline(cnfg_file, myText)) {
        cout << myText;
  }

  cnfg_file.close();
}
