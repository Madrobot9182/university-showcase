//
// Created by ryan on 15/09/23.
//
#include <stdio.h>
#include <stdlib.h>

int LIMIT = 100000;

void get_books(int* hardb, int* softb, int* ebook){
    printf("How many hardcover books are you buying? ");
    if (scanf("%d", hardb) != 1 || *hardb < 0 || *hardb >= LIMIT)
        exit(1);
    printf("How many softcover books are you buying? ");
    if (scanf("%d", softb) != 1 || *softb < 0 || *softb >= LIMIT)
        exit(1);
    printf("How many ebooks are you buying? ");
    if (scanf("%d", ebook) != 1 || *ebook < 0 || *ebook >= LIMIT)
        exit(1);
}

int coupons(const int hardb, const int softb){
    int total = 0;

    if (hardb >= 2)
        total += 5;
    if (softb >= 4)
        total += 10;
    if ((hardb + softb) >= 6)
        total += 20;

    return total;
}

double discounts(const int hardb, const int softb, const int ebook, const int cost){
    double percentage = 0;

    if (hardb >= 1 && softb >= 1 && ebook >= 1)
        percentage += 3;
    if (ebook >= 3)
        percentage += 4;
    if (cost > 75)
        percentage += 10;
    if (cost > 150)
        percentage += 15;

    //return preentage as decimal
    return percentage / 100;
}

int main(){
    int hardb, softb, ebook = 0;

    get_books(&hardb, &softb, &ebook);
    double cost = (hardb * 15) + (softb * 12) + (ebook * 7);

    int coupon = coupons(hardb, softb);
    double discount = discounts(hardb, softb, ebook, cost - coupon);

    //first subtract coupons, then apply discounts
    cost = (cost - coupon) * (1 - discount);
    printf("Order total: $%.2f\n", cost);

    return 0;
}
