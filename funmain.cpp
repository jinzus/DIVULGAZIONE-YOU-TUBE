#include <iostream>
#include "ExponentiateBoss.h"
using namespace std;
using namespace youtubetutorial;

int main()
{
    int n_items;
    cout << "Quanti valori vogliamo ottenere: ";
    cin >> n_items;
    if (n_items == 0)
    {
        cout << "0 items e' non valido";
        return 0;
    }
    ExponentiateBoss expo(n_items);
    cout << expo;
    return 0;
}

