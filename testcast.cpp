
#include <bits/stdc++.h>
#define fle(i, a, b) for (int (i) = (a); (i) <= (b); (i)++)
#define flt(i, a, b) for (int (i) = (a); (i) < (b); (i)++)
#define fgt(i, a, b) for (int (i) = (a); (i) > (b); (i)--)
#define fge(i, a, b) for (int (i) = (a); (i) >= (b); (i)--)
#define log(var, sep) (cerr << #var << " = " << (var) << (sep))
#define pb push_back
#define nl (cout << endl)
#define pf first
#define ps second

using namespace std;

using ll = long long;
using ii = pair<ll, ll>;
using vi = vector<ll>;
using vp = vector< ii >;

const int MAX { 1000010 }; // 10^6 + 10
const ll oo { (1ll << 62) }; // 2^62
const ll MOD { 1000000007 }; // 10^9 + 7
const double PI = acos(-1.0); // 3.14
int n, m;
typedef struct s_xyzzy {
    int num;
    char name[20];
    float secret;
} xyzzy;


int func(void* xd){
    xyzzy ka = *(xyzzy*)xd;
    // int x = *((int*)(&xd));
   

    cout << ka.num << endl;
}

int main () {
    xyzzy plugh;
    plugh.num = 42;
    strcpy (plugh.name, "paxdiablo");
    plugh.secret = 3.141592653589;
    string xd = "ola";
    func(&plugh);
    return 0;
}
