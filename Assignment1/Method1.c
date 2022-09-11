#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define ll long long int
#define rep(i,a,N) for(ll i=a;i<N;i++)

ll seed_using_xor(ll x){
    return time(0)^x;
}
ll sign_num(ll x){
    srand(seed_using_xor(x));
    ll  res=(rand()%2+2)%2;
    if(!res) return -1;
    return 1;
}
ll *numgen(ll t,ll *arr,ll siz){
    srand(seed_using_xor(t*siz));
    rep(i,0,siz) arr[i]=(rand()%2+2)%2;
    return arr;
}
ll bin_dec(ll *num,ll sz){
    ll x=0;
    rep(i,0,sz) x+=num[i]*(1LL<<i);
    return x;
}
void solve(){
    ll N;
    scanf("%lld",&N);
    ll nums[N];
    rep(i,0,N){
        ll sz=20+(rand()%13+13)%13;
        ll arr[sz];
        ll *ptr=numgen(i,arr,sz);
        nums[i]=sign_num(i*i)*bin_dec(ptr,sz);
    }
    rep(i,0,N)printf("%lld\n",nums[i]);


}
int main(){  
    
    solve();
    return 0;

}

