#pragma once

#include "headers.h"

int f[2005][2005];

int lcs(const string &a,const string &b) {
    if(a.length()>2000||b.length()>2000)return 0;
    memset(f,0,sizeof(f));
    for(int i=1; i<=a.length(); i++)
        for(int j=1; j<=b.length(); j++) {
            if(a[i-1]==b[j-1])f[i][j]=f[i-1][j-1]+1;
            else f[i][j]=max(f[i-1][j],f[i][j-1]);
        }
    return f[a.length()][b.length()];
}