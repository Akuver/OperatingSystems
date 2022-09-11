#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define ll long long int
#define MOD 1e9+7
#define rep(i,a,N) for(ll i=a;i<N;i++)

ll fibs[1000005];
ll modu=(1LL<<32);
void foo(){
    fibs[0]=0;
    fibs[1]=1;
    rep(i,2,1000005) fibs[i]=(fibs[i-1]%modu+fibs[i-2]%modu+modu)%modu;
}
ll j=9739,k=23209;

void solve(){
    ll N;
    scanf("%lld",&N);
    ll nums[N];
    rep(i,0,N){
        nums[i]=(fibs[i+j]^fibs[i+k])%modu;
    }
    rep(i,0,N)printf("%lld\n",nums[i]);

}
int main(){  
    foo();
    solve();
    return 0;

}

