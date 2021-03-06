//
//  readFile.cpp
//  AlgorithmProject
//
//  Created by 李勁璋 on 2014/6/9.
//  Copyright (c) 2014年 ntu. All rights reserved.
//

#include "readFile.h"
#include <fstream>
#include <math.h>
#include <algorithm>
bool greater_brightX (BumpNode i,BumpNode j) { return (i.absolutedX2 > j.absolutedX2); }
bool smaller_bbottomY (BumpNode i,BumpNode j) { return (i.absolutedY2 < j.absolutedY2); }
bool smaller_bleftX (BumpNode i,BumpNode j) { return (i.absolutedX1 < j.absolutedX1); }

ReadFile::ReadFile()
{
  //  char buffer[200];
    fstream infile("test144.in");
    //fstream infile;
   // infile.open("test144.in", ios::in); //read test file
    if (!infile.is_open()) {
        printf("read file error");
    }
    
    Boundary bound;
    int width, spacing, num_driver, num_bump;
    string junk1, junk2, junk3, junk4, junk5;
    
    w = width;
    s = spacing;
    
    infile >> junk1 >> bound.leftX >> bound.bottomY >> bound.rightX >> bound.topY;
    printf("%s", junk1.c_str());
    infile >> junk2 >> width;
    infile >> junk3 >> spacing;
    infile >> junk4 >> num_driver;
    
   // dsize = new vector<Dsize>[num_driver - 1];
  //  bsize(num_bump-1);

   // vector<int> num_d(num_driver-1);
   // vector<int*> num_b(num_bump-1);
    DriverNode input_d;
    int id, left, right, bottom, top;
    
    while (infile >> id >> left >> bottom >> right >> top){
        input_d.did = id;
        input_d.dleftX = left;
        input_d.drightX = right;
        input_d.dbottomY = bottom;
        input_d.dtopY = top;
        driver.push_back(input_d);
        
        if (input_d.did == num_driver)
            break;
    } // end of reading the layout of drivers
    infile >> junk5 >> num_bump;
    
    while(infile >> id >> left >> bottom >> right >> top){
        BumpNode *input_b = new BumpNode(0, 0, UnDirectRoute, false);
        input_b->wireId = &driver[id].did;
        input_b->absolutedX1 = left;
        input_b->absolutedX2 = right;
        input_b->absolutedY1 = bottom;
        input_b->absolutedY2 = top;
        bump.push_back(*input_b);
    } //end of reading the layout of bumps
    infile.close();
    
    alignment(num_bump);
}
void ReadFile::alignment(int bumpNum)
{
    
    vector<BumpNode>bump_dup = bump;

    int n = sqrt(bumpNum) / 2;
    numBvec = n;
    bvec = new vector<BumpNode>[n]; //create n vectors to store rings
    
    //if indicator id true:
    for( int j = 0; j <= n; j++){
        for(int i = 0; bump_dup[i].absolutedY2 < bump_dup[i+1].absolutedY2; i++){
            bvec[j].push_back(bump_dup[i]);
            bump_dup.erase(bump_dup.begin() + i);
        }
        sort(bump_dup.begin(), bump_dup.end(), greater_brightX);
        for(int i = 0; bump_dup[i].absolutedX2 < bump_dup[i+1].absolutedX2; i++){
            bvec[j].push_back(bump_dup[i]);
            bump_dup.erase(bump_dup.begin() + i);
        }
        sort(bump_dup.begin(), bump_dup.end(), smaller_bbottomY);
        for(int i = 0; bump_dup[i].absolutedY2 < bump_dup[i+1].absolutedY2; i++){
            bvec[j].push_back(bump_dup[i]);
            bump_dup.erase(bump_dup.begin() + i);
            sort(bump_dup.begin(), bump_dup.end(), smaller_bleftX);
            for(int i = 0; bump_dup[i].absolutedX1 < bump_dup[i+1].absolutedX1; i++){
                bvec[j].push_back(bump_dup[i]);
                bump_dup.erase(bump_dup.begin() + i);
            } //run LCS algorithms with all bvec
        }
    }
}
void ReadFile::LCS(vector<DriverNode> driver_vec, vector<BumpNode>*bump_vec, int num_driver, int n){
    //lcs_bump dpary[num_driver][n];//DP table
    *dpary = new lcs_bump[num_driver];
    for (int i = 0; i < num_driver; ++i) {
        dpary[i] = new lcs_bump[n];
    }
    for(int i = 0; i <= num_driver; i++)
        dpary[i][0].cs = 0;
    for(int i = 0; i <= n; i++)
        dpary[0][i].cs = 0;
    
    for(int i = 1; i <= num_driver; i++){
        for(int j = 1; j <= n; j++){
            if(driver_vec[i].did == *(*bump_vec)[j].wireId)
            {
                dpary[i][j].cs = dpary[i-1][j-1].cs + 1;
                dpary[i][j].prev = &dpary[i-1][j-1].cs;
            }
            else
            {
                if(dpary[i-1][j].cs < dpary[i][j-1].cs)
                {
                    dpary[i][j].cs = dpary[i][j-1].cs;
                    dpary[i][j].prev = &dpary[i][j-1].cs;
                    
                }
                else
                {
                    dpary[i][j].cs = dpary[i-1][j].cs;
                    dpary[i][j].prev = &dpary[i-1][j].cs;
                    
                }
            }
        }
    }
    label_LCS(driver_vec.size(),bump_vec->size(), bump_vec);
//    if( driver_vec.size() == 0 || bump_vec.size() == 0)
//        return;
//    if(dpary[i][j].prev == *dpary[i-1][j-1].cs)
//        label_LCS(i-1,j-1);
//    bump_vec[j].lcsType = DirectRoute;
//    else if(dpary[i][j].prev == *dpary[i-1][j].cs)
//        label_LCS(i-1,j);
//    else if(dpary[i][j].prev == *dpary[i][j-1].cs)
//        label_LCS(i,j-1);
//	for(int u = 0; u < j; u++){
//		if(bump_vec[u].lcsType != DirectRoute)
//			bump_vec[u].lcsType = UnDirectRoute;
//	}
}


void ReadFile::label_LCS(int i, int j, vector<BumpNode>*bump_vec)
{
    if(i == 0 || j == 0)
        return;
    if(dpary[i][j].prev == &dpary[i-1][j-1].cs){
        label_LCS(i-1,j-1,bump_vec);
        (*bump_vec)[j].lcsType = DirectRoute;
    }else if(dpary[i][j].prev == &dpary[i-1][j].cs)
        label_LCS(i-1,j, bump_vec);
    else if(dpary[i][j].prev == &dpary[i][j-1].cs)
        label_LCS(i,j-1, bump_vec);
	for(int u = 0; u < j; u++){
		if((*bump_vec)[u].lcsType != DirectRoute)
			(*bump_vec)[u].lcsType = UnDirectRoute;
	}
}

