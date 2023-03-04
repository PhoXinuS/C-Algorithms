#include <cassert>
#include <cstdlib>
#include <vector>
#include "kol.h"

using namespace std;

// In this code every queue of clients is kept as double-linked list
// in every counter(queue) only first and last client are kept
// to solve specific issues first and last client in every queue remembers an index of a queue

typedef interesant* client;
struct queue {
    client first = NULL;
    client last = NULL;
};

vector<queue> counters;
int numberOfClients = 0;

void otwarcie_urzedu(int m) {
    for (int i = 0; i < m; ++i) {
        queue newQueue;
        newQueue.first = NULL;
        newQueue.last = NULL;
        counters.push_back(newQueue);
    }
}


// Allocate a new client and connect it to the end of a given queue
interesant *nowy_interesant(int k) {
    interesant* newClient = (interesant*)malloc(sizeof(struct interesant));
    newClient->client1 = NULL;
    newClient->client2 = NULL;
    newClient->counterIndex = k;
    newClient->clientNumber = numberOfClients;
    numberOfClients++;

    // connect to the last client
    if(counters[k].first != NULL) {
        newClient->client1 = counters[k].last;
        if (counters[k].last->client1 == NULL) {
            counters[k].last->client1 = newClient;
        }
        else {
            counters[k].last->client2 = newClient;
        }
        counters[k].last = newClient;
    }
    else {
        counters[k].first = newClient;
        counters[k].last = newClient;
    }
    return newClient;
}


int numerek(interesant *i) {
    return i->clientNumber;
}


// Remove first client from a queue if any exist
interesant *obsluz(int k) {
    client firstClient = counters[k].first;
    if(firstClient == NULL) {
        return NULL;
    }

    // erase connection to the served client and set next one as first
    if (firstClient->client1 != NULL) {
        if(firstClient->client1->client1 == firstClient) {
            firstClient->client1->client1 = NULL;
        }
        else {
            firstClient->client1->client2 = NULL;
        }
        counters[k].first = firstClient->client1;
    }
    else if (firstClient->client2 != NULL){
        if(firstClient->client2->client1 == firstClient) {
            firstClient->client2->client1 = NULL;
        }
        else {
            firstClient->client2->client2 = NULL;
        }
        counters[k].first = firstClient->client2;
    }
    else {
        counters[k].first = NULL;
        counters[k].last = NULL;
    }

    if(counters[k].first != NULL) {
        counters[k].first->counterIndex = firstClient->counterIndex;
    }
    return firstClient;
}


void zmiana_okienka(interesant *i, int k) {

    // if client is in the middle of a queue
    if(i->client1 != NULL && i->client2 != NULL) {
        if(i->client1->client1 == i) {
            i->client1->client1 = i->client2;
        }
        else {
            i->client1->client2 = i->client2;
        }

        if(i->client2->client1 == i) {
            i->client2->client1 = i->client1;
        }
        else {
            i->client2->client2 = i->client1;
        }
    }
    // if client is an only element in a queue
    else if(i->client1 == NULL && i->client2 == NULL) {
        counters[i->counterIndex].first = NULL;
        counters[i->counterIndex].last = NULL;
    }
    // if client is in the start or the end
    else if(i->client1 != NULL || i->client2 != NULL) {
        if(counters[i->counterIndex].first == i) {
            if(i->client1 != NULL) {
                if(i->client1->client1 == i) {
                    i->client1->client1 = NULL;
                }
                else {
                    i->client1->client2 = NULL;
                }
                counters[i->counterIndex].first = i->client1;
            }
            else {
                if(i->client2->client1 == i) {
                    i->client2->client1 = NULL;
                }
                else {
                    i->client2->client2 = NULL;
                }
                counters[i->counterIndex].first = i->client2;
            }
            counters[i->counterIndex].first->counterIndex = i->counterIndex;
        }
        else {
            if(i->client1 != NULL) {
                if(i->client1->client1 == i) {
                    i->client1->client1 = NULL;
                }
                else {
                    i->client1->client2 = NULL;
                }
                counters[i->counterIndex].last = i->client1;
            }
            else {
                if(i->client2->client1 == i) {
                    i->client2->client1 = NULL;
                }
                else {
                    i->client2->client2 = NULL;
                }
                counters[i->counterIndex].last = i->client2;
            }
            counters[i->counterIndex].last->counterIndex = i->counterIndex;
        }
    }

    // add to a new queue
    if(counters[k].last == NULL) {
        i->client1 = NULL;
        counters[k].first = i;
    }
    else {
        i->client1 = counters[k].last;
        if(counters[k].last->client1 == NULL) {
            counters[k].last->client1 = i;
        }
        else {
            counters[k].last->client2 = i;
        }
    }
    i->client2 = NULL;
    i->counterIndex = k;
    counters[k].last = i;
}


void zamkniecie_okienka(int k1, int k2) {
    if(counters[k1].first == NULL) {
        return;
    }

    // if new queue is empty
    if(counters[k2].first == NULL) {
        counters[k2].first = counters[k1].first;
    }
    // if new queue is not empty
    else {
        if(counters[k2].last->client1 == NULL) {
            counters[k2].last->client1 = counters[k1].first;
        }
        else {
            counters[k2].last->client2 = counters[k1].first;
        }

        if(counters[k1].first->client1 == NULL) {
            counters[k1].first->client1 = counters[k2].last;
        }
        else {
            counters[k1].first->client2 = counters[k2].last;
        }
    }
    counters[k2].last = counters[k1].last;

    // assign new queue index to its elements
    counters[k1].first->counterIndex = k2;
    counters[k1].first = NULL;
    counters[k1].last->counterIndex = k2;
    counters[k1].last = NULL;
}


// This function works by creating 2 vectors filled simultaneously in 2 directions from i1
// until one comes across i2, and then it erases connections of i1 and i2
vector<interesant *> fast_track(interesant *i1, interesant *i2) {

    // Find a vector of clients from i1 to i2
    vector<client> edge1;
    vector<client> edge2;
    edge1.push_back(i1);
    edge2.push_back(i1);
    if(i1 != i2) {
        edge1.push_back(i1->client1);
        edge2.push_back(i1->client2);
    }

    while(edge1.back() != i2 && edge2.back() != i2) {
        if(i1 == i2) {
            break;
        }

        if (edge1.back() != NULL) {
            client client1 = edge1.back()->client1;
            client client2 = edge1.back()->client2;
            if(edge1.end()[-2] != client1) {
                edge1.push_back(client1);
            }
            else {
                edge1.push_back(client2);
            }
        }

        if(edge2.back() != NULL) {
            client client1 = edge2.back()->client1;
            client client2 = edge2.back()->client2;
            if(edge2.end()[-2] != client1) {
                edge2.push_back(client1);
            }
            else {
                edge2.push_back(client2);
            }
        }
    }

    if(edge2.back() == i2) {
        edge1 = edge2;
    }

    client beforeFirst;
    client pastLast;

    if(i1 == i2) {
        beforeFirst = edge1[0]->client1;
        pastLast = edge1[0]->client2;
    }
    else {
        if(edge1[0]->client1 != edge1[1]) {
            beforeFirst = edge1[0]->client1;
        }
        else {
            beforeFirst = edge1[0]->client2;
        }

        if(edge1.back()->client1 != edge1.end()[-2]) {
            pastLast = edge1.back()->client1;
        }
        else {
            pastLast = edge1.back()->client2;
        }
    }

    // Erase connections

    // if vector has an entire queue inside
    if(beforeFirst == NULL && pastLast == NULL) {
        counters[edge1[0]->counterIndex].first = NULL;
        counters[edge1[0]->counterIndex].last = NULL;
    }
    // if vector is from the bottom to somewhere in the middle of a queue
    else if(beforeFirst == NULL) {
        if(pastLast->client1 == edge1.back()) {
            pastLast->client1 = NULL;
        }
        else {
            pastLast->client2 = NULL;
        }
        pastLast->counterIndex = edge1[0]->counterIndex;

        if (edge1[0] == counters[edge1[0]->counterIndex].first) {
            counters[edge1[0]->counterIndex].first = pastLast;
        }
        else {
            counters[edge1[0]->counterIndex].last = pastLast;
        }
    }
    // if vector is from somewhere in the middle to the end of a queue
    else if(pastLast == NULL) {
        if(beforeFirst->client1 == edge1[0]) {
            beforeFirst->client1 = NULL;
        }
        else {
            beforeFirst->client2 = NULL;
        }
        beforeFirst->counterIndex = edge1.back()->counterIndex;

        if (edge1.back() == counters[edge1.back()->counterIndex].first) {
            counters[edge1.back()->counterIndex].first = beforeFirst;
        }
        else {
            counters[edge1.back()->counterIndex].last = beforeFirst;
        }
    }
    // if vector is only somewhere in the middle of a queue
    else {
        if(beforeFirst->client1 == edge1[0]) {
            beforeFirst->client1 = pastLast;
        }
        else {
            beforeFirst->client2 = pastLast;
        }
        if(pastLast->client1 == edge1.back()) {
            pastLast->client1 = beforeFirst;
        }
        else {
            pastLast->client2 = beforeFirst;
        }
    }
    return edge1;
}

void naczelnik(int k) {
    swap(counters[k].first, counters[k].last);
}

// loop through all counters and its clients and add clients to a cumulative vector
vector<interesant *> zamkniecie_urzedu() {
    vector<client> allClients;
    int counter = 0;
    for(queue i: counters) {
        counter++;

        client currentClient = i.first;
        client prevClient = NULL;
        if(currentClient != NULL) {
            allClients.push_back(currentClient);
        }

        while (currentClient != i.last) {
            if(currentClient->client1 != prevClient) {
                prevClient = currentClient;
                currentClient = currentClient->client1;
            }
            else {
                prevClient = currentClient;
                currentClient = currentClient->client2;
            }

            if(currentClient != NULL) {
                allClients.push_back(currentClient);
            }
        }
    }
    return allClients;
}