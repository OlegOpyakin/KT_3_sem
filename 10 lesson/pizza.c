#include <pthread.h>
#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define NUM_PIZZAS 25 // total nubmer of pizzas, can be changed
#define BAKERY_SIZE 10 // size of the bakery, can be changed
#define PIZZA_SIZE 5 // pizza size, can't be changed

//----------------------------- hoar -----------------------------

struct hoare{
    pthread_mutex_t mutex;
    char pizza[BAKERY_SIZE][PIZZA_SIZE]; // 2 dimensional massive for pizzas in bakery

    int pizza_to_get_d[BAKERY_SIZE]; // indicator mas, to decide if we can put "d" on pizza
    int pizza_to_get_o[BAKERY_SIZE]; // indicator mas, to decide if we can put "o" on pizza

    int ingridients_in_pizza[BAKERY_SIZE]; // mas, to show number of ingridients in pizza
    int pizza_to_be_taken[BAKERY_SIZE]; // indicator mas, to show if deliverer can take this pizza
    int is_pizza_in_work[BAKERY_SIZE]; // indicator mas, to show which pizzas are we working with

    char couriers_pizza[PIZZA_SIZE]; // string for pizza, that couriuer has taken
    char consumer_pizza[PIZZA_SIZE]; // pizza that consumer eats
     
    int has_pizza; // flag if courier has pizza
    int worked_pizzas; // number of pizzas ozzi and di worked with
};

//----------------------------- Put d -----------------------------

int PutD(struct hoare* st){
    int if_we_put = 0;

    pthread_mutex_lock(&(st->mutex));

    for(int i = 0; i < BAKERY_SIZE; i++){
        if(st->worked_pizzas < NUM_PIZZAS && st->is_pizza_in_work[i] == 0){
            st->worked_pizzas += 1;
            st->is_pizza_in_work[i] = 1;
        }
    }

    for(int i = 0; i < BAKERY_SIZE; i++){ // place d

        if(st->pizza_to_get_d[i] == 1 && st->ingridients_in_pizza[i] < 4 && st->is_pizza_in_work[i] == 1){

            strcat(st->pizza[i], "d");

            st->pizza_to_get_d[i] = 0;
            st->pizza_to_get_o[i] = 1;
            st->ingridients_in_pizza[i] += 1;

            printf("placed d to pizza[%d] = %s\n", i, st->pizza[i]);
            //printf("ingridients = %d\n", st->ingridients_in_pizza[i]);
            if_we_put = 1;
            break;
        }
    }
    pthread_mutex_unlock(&(st->mutex));
    return if_we_put;
}   

//----------------------------- Put o -----------------------------

int PutO(struct hoare* st) 
{
    int if_we_put = 0;

    pthread_mutex_lock(&(st->mutex));

    for(int i = 0; i < BAKERY_SIZE; i++){ // place o

        if(st->pizza_to_get_o[i] == 1 && st->ingridients_in_pizza[i] < 4 && st->is_pizza_in_work[i] == 1){
            strcat(st->pizza[i], "o");
            st->ingridients_in_pizza[i] += 1;

            if(st->ingridients_in_pizza[i] == 4){ // if pizza ready

                st->pizza_to_get_o[i] = 0;
                st->pizza_to_get_d[i] = 0;
                st->pizza_to_be_taken[i] = 1;

                printf("placed final o to pizza[%d] = %s\n", i, st->pizza[i]);
                //printf("ingridients = %d\n", st->ingridients_in_pizza[i]);

                if_we_put = 1;
                break;
            }
            else{
                st->pizza_to_get_o[i] = 0;
                st->pizza_to_get_d[i] = 1;

                printf("placed o to pizza[%d] = %s\n", i, st->pizza[i]);
                //printf("ingridients = %d\n", st->ingridients_in_pizza[i]);

                if_we_put = 1;
                break;
            }
        }
    }
    pthread_mutex_unlock(&(st->mutex));
    
    return if_we_put;
}

//----------------------------- take pizza -----------------------------

int Take(struct hoare* st) 
{
    int if_we_take = 0;

        pthread_mutex_lock(&(st->mutex));

        for(int i = 0; i < BAKERY_SIZE; i++){// take pizza
            if(st->pizza_to_be_taken[i] == 1 && st->has_pizza == 0){

                strcpy(st->couriers_pizza, st->pizza[i]);
                st->pizza[i][0] = 0;

                st->pizza_to_be_taken[i] = 0;
                st->pizza_to_get_d[i] = 1;
                st->pizza_to_get_o[i] = 0;
                st->has_pizza = 1;
                st->ingridients_in_pizza[i] = 0;
                st->is_pizza_in_work[i] = 0;

                printf("taken pizza %d\n", i);
                if_we_take = 1;
                break;
            }
            
        }
        pthread_mutex_unlock(&(st->mutex));
    
    return if_we_take;
}

//----------------------------- eat pizza -----------------------------

int Eat(struct hoare* st) 
{
    int if_we_eat = 0;
    pthread_mutex_lock(&(st->mutex));

    if(st->has_pizza == 1){
        st->has_pizza = 0;
        printf("customer eated pizza: %s\n", st->couriers_pizza);
        for(int j = 0; j < 4; j++) st->couriers_pizza[j] = 0;
        if_we_eat = 1;
    }
    pthread_mutex_unlock(&(st->mutex));
    return if_we_eat;
}


//----------------------------- di -----------------------------

void* povar_d(void* str_void) 
{
    struct hoare* st = (struct hoare*) str_void;
    int iterator = 0;

    while(iterator < NUM_PIZZAS * 2){
        iterator += PutD(st);
    }
    return NULL;
}

//----------------------------- ozzie -----------------------------

void* povar_o(void* str_void) 
{
    struct hoare* st = (struct hoare*) str_void;
    int iterator = 0;

    while(iterator < NUM_PIZZAS * 2){
        iterator += PutO(st);
    }
    return NULL;
}

//----------------------------- deliverer -----------------------------

void* deliverer(void* str_void) 
{
    struct hoare* st = (struct hoare*) str_void;
    int iterator = 0;
    while(iterator < NUM_PIZZAS){
        iterator += Take(st);
    }
    return NULL;
}

//----------------------------- consumer -----------------------------

void* eat(void* str_void) 
{
    struct hoare* st = (struct hoare*) str_void;
    int iterator = 0;

    while(iterator < NUM_PIZZAS){
        int tmp = Eat(st);
        iterator += tmp;
        if(tmp == 1) printf("Number of eaten pizzas: %d\n", iterator);
    }
    return NULL;
}


//----------------------------- main -----------------------------

int main(){
    pthread_t tid[4];
    struct hoare st;

    pthread_mutex_init(&(st.mutex), NULL);

    for(int i = 0; i < BAKERY_SIZE; i++){
        st.pizza_to_get_d[i] = 1;
        st.pizza_to_get_o[i] = 0;
        st.pizza_to_be_taken[i] = 0;
        st.ingridients_in_pizza[i] = 0;
        st.is_pizza_in_work[i] = 0;
    }

    st.has_pizza = 0;
    st.worked_pizzas = 0;

    pthread_create(&tid[0], NULL, povar_d, &st);
    pthread_create(&tid[1], NULL, povar_o, &st);
    pthread_create(&tid[2], NULL, deliverer, &st);
    pthread_create(&tid[3], NULL, eat, &st);

    for(int i = 0; i < 4; i++) pthread_join(tid[i], NULL);

    pthread_mutex_destroy(&(st.mutex));
    return 0;
}