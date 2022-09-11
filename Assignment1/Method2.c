#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define ll long long int
#define MOD 1e9+7
#define rep(i,a,N) for(ll i=a;i<N;i++)
ll X_not=0,a=(1LL<<31)-1,b=48271,c=MOD;
void solve(){
    ll N;
    scanf("%lld",&N);
    ll nums[N+1];
    nums[0]=X_not;
    rep(i,1,N+1) nums[i]=(a*nums[i-1]+b)%c;
    rep(i,1,N+1) printf("%lld\n",nums[i]);

}
int main(){  
    
    solve();
    return 0;

}

