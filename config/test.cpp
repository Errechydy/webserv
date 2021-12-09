#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main () {
  // string myText;
  // ifstream cnfg_file("filename.txt");

  // while (getline(cnfg_file, myText)) {
  //       cout << myText;
  // }

  // cnfg_file.close();

  std::string str_tmp = "    hello";

  int len = str_tmp.size();
  int first = 0;
  while (str_tmp[first])
  {
      if(str_tmp[first] != 32 && str_tmp[first] != 9)
          break ;

      std::cout << "char : |" << str_tmp[first] << "|" << std::endl;


      first++;
  }
  return 0;
}
