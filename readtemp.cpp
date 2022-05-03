/* https://stackoverflow.com/a/44611186 */

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <array>

using namespace std; 

int main()
{
    string command("python3 simple.py 2>&1");

    array<char, 128> buffer;
    string result;

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        cerr << "Couldn't read temperature!" << endl;
        return 1;
    }
    while (fgets(buffer.data(), 128, pipe) != NULL) {
        result += buffer.data();
    }
    pclose(pipe);

    cout << result << endl;
    //cout << returnCode << endl;

    return 0;
}
