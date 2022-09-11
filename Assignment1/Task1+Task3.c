#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define ll long long int
#define MOD 1e9+7
#define rep(i,a,N) for(ll i=a;i<N;i++)

ll nums[1000005];
ll N;

void swap(ll* a, ll* b){ 
    ll t = *a; 
    *a = *b; 
    *b = t; 
} 
ll partition(ll arr[],ll low,ll high){ 
    ll pivot=arr[high];
    ll i=(low-1); 
    for(ll j=low;j<=high-1;j++){ 
        if (arr[j]<pivot){ 
            i++; 
            swap(&arr[i],&arr[j]); 
        } 
    } 
    swap(&arr[i + 1],&arr[high]); 
    return(i+1); 
} 
void quickSort(ll arr[],ll low,ll high){ 
    if (low<high){ 
        ll pi=partition(arr,low,high); 
        quickSort(arr,low,pi-1); 
        quickSort(arr,pi+1,high); 
    } 
} 
ll bin_search(ll elem,ll hashes[],ll sz){
    quickSort(hashes,0,sz-1);
    ll l=0,r=sz;
    while(l<r){
        ll m=(l+r)/2;
        if(hashes[m]<=elem) l=m+1;
        else r=m;
    }
    if(!l)  return 0;
    if(hashes[l-1]==elem) return 1;
    return 0;
}

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
ll hash(ll x){
    x = ((x>>16)^x)*0x45d9f3b;
    x = ((x>>16)^x)*0x45d9f3b;
    x = (x>>16)^x;
    return x;
}
void setgen(ll arr[],ll sz){
    ll hashes[sz];
    ll narr[sz],p=0;
    rep(i,0,sz){
        ll cur_hash=hash(arr[i]);
        if(!bin_search(cur_hash,hashes,p)) narr[p]=arr[i],hashes[p]=cur_hash,p++;
    }
    rep(i,0,p) printf("%lld. %lld\n",i+1,narr[i]);
}
void M1(){
    printf("%s\n","Method1");
    ll nums[N];
    rep(i,0,N){
        ll sz=20+(rand()%13+13)%13;
        ll arr[sz];
        ll *ptr=numgen(i,arr,sz);
        nums[i]=sign_num(i*i)*bin_dec(ptr,sz);
    }
    //rep(i,0,N)printf("%lld\n",nums[i]);
    setgen(nums,N);
}
void M2(){
    printf("%s\n","Method2");
    ll X_not=0,a=(1LL<<31)-1,b=48271,c=MOD;
    ll nums[N+1];
    nums[0]=X_not;
    rep(i,1,N+1) nums[i]=(a*nums[i-1]+b)%c;
    //rep(i,1,N+1) printf("%lld\n",nums[i]);
    setgen(nums,N);
}
void M3(){
    printf("%s\n","Method3");
    ll fibs[1000005];
    ll modu=(1LL<<32);
    ll j=9739,k=23209;
    fibs[0]=0;
    fibs[1]=1;
    rep(i,2,1000005) fibs[i]=(fibs[i-1]%modu+fibs[i-2]%modu+modu)%modu;
    ll nums[N];
    rep(i,0,N){
        nums[i]=(fibs[i+j]^fibs[i+k])%modu;
    }
    //rep(i,0,N)printf("%lld\n",nums[i]);
    setgen(nums,N);
}

int main(){
    scanf("%lld",&N);
    M1();M2();M3();
    return 0;
}