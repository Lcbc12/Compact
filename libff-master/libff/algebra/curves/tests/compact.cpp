/**
 *****************************************************************************
 * @author     Lucas GICQUEL
 *****************************************************************************/

#include <libff/algebra/curves/edwards/edwards_pp.hpp>
#include <libff/common/profiling.hpp>
#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libff/algebra/curves/mnt/mnt4/mnt4_pp.hpp>
#include <libff/algebra/curves/mnt/mnt6/mnt6_pp.hpp>

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>

#include <unistd.h>

#define PATH_DIR "results/"

using namespace libff;
using namespace std;

/**
 * Read data, a Fr point, in the file specifies in path
 * @param path file where the data is written
 * @return Fr point stored in file 
 */
template<typename ppT>
Fr<ppT> readFr(string path)
{
    //TODO exceptions
    Fr<ppT> toReturn;
    ifstream is_file(PATH_DIR + path, ios::binary | ios::in);
    is_file.read((char*) &toReturn, sizeof(Fr<ppT>));
    is_file.close();

    return toReturn;
}

/**
 * Read data, a G1 point, in the file specifies in path
 * @param path file where the data is written
 * @return G1 point stored in file 
 */
template<typename ppT>
G1<ppT> readG1(string path)
{
    //TODO exceptions
    alt_bn128_Fq X;
    alt_bn128_Fq Y;
    alt_bn128_Fq Z;

    ifstream is_file(PATH_DIR + path, ios::binary | ios::in);
    is_file.read((char*) &X, sizeof(alt_bn128_Fq));
    is_file.read((char*) &Y, sizeof(alt_bn128_Fq));
    is_file.read((char*) &Z, sizeof(alt_bn128_Fq));
    is_file.close();
    return alt_bn128_G1(X, Y, Z);
}

/**
 * Read data, a G1 point, in the file specifies in path at a certain spot
 * of the file specifies by position
 * @param path file where the data is written
 * @param position spot where the data to read is stored
 * @return G1 point stored in file at the position <position>
 */
template<typename ppT>
G1<ppT> readG1Pos(string path, unsigned long long int position)
{
    //TODO exceptions
    alt_bn128_Fq X;
    alt_bn128_Fq Y;
    alt_bn128_Fq Z;

    ifstream is_file(PATH_DIR + path, ios::binary | ios::in);
    is_file.seekg(sizeof(G1<ppT>) * position, is_file.beg);
    is_file.read((char*) &X, sizeof(alt_bn128_Fq));
    is_file.read((char*) &Y, sizeof(alt_bn128_Fq));
    is_file.read((char*) &Z, sizeof(alt_bn128_Fq));
    is_file.close();
    return alt_bn128_G1(X, Y, Z);
}

/**
 * Read data, a G2 point, in the file specifies in path
 * @param path file where the data is written
 * @return G2 point stored in file 
 */
template<typename ppT>
G2<ppT> readG2(string path)
{
    //TODO exceptions
    alt_bn128_Fq2 X;
    alt_bn128_Fq2 Y;
    alt_bn128_Fq2 Z;

    ifstream is_file(PATH_DIR + path, ios::binary | ios::in);
    is_file.read((char*) &X, sizeof(alt_bn128_Fq2));
    is_file.read((char*) &Y, sizeof(alt_bn128_Fq2));
    is_file.read((char*) &Z, sizeof(alt_bn128_Fq2));
    is_file.close();
    return alt_bn128_G2(X, Y, Z);
}

/**
 * Write data, a Fr point, in the file specifies in path
 * @param path file where the data is written
 * @param data Fr point to write in the file
 */
template<typename ppT>
void writeFr(string path, Fr<ppT> data)
{
    //TODO exceptions
    ofstream os_file(PATH_DIR + path, ios::binary| ios::out);
    os_file.write((char*) &data, sizeof(Fr<ppT>));
    os_file.close();
}

/**
 * Write data, a G1 point, in the file specifies in path
 * @param path file where the data is written
 * @param data G1 point to write in the file
 */
template<typename ppT>
void writeG1(string path, G1<ppT> data)
{
    //TODO exceptions
    ofstream os_file(PATH_DIR + path, ios::binary| ios::out);
    alt_bn128_Fq p = data.X;

    os_file.write((char*) &p, sizeof(alt_bn128_Fq));
    p = data.Y;
    os_file.write((char*) &p, sizeof(alt_bn128_Fq));
    p = data.Z;
    os_file.write((char*) &p, sizeof(alt_bn128_Fq));
    os_file.close();
}

/**
 * Write data, a G2 point, in the file specifies in path
 * @param path file where the data is written
 * @param data G2 point to write in the file
 */
template<typename ppT>
void writeG2(string path, G2<ppT> data)
{
    //TODO exceptions
    ofstream os_file(PATH_DIR + path, ios::binary| ios::out);
    alt_bn128_Fq2 p = data.X;

    os_file.write((char*) &p, sizeof(alt_bn128_Fq2));
    p = data.Y;
    os_file.write((char*) &p, sizeof(alt_bn128_Fq2));
    p = data.Z;
    os_file.write((char*) &p, sizeof(alt_bn128_Fq2));
    os_file.close();
}

/**
 * Initialize all necessary metadata to run the protocol of Proof of
 * Retrievability :
 *  - secret key: sk
 *  - public key: pk
 *  - file identifier: name
 *  - s G1 points: u
 * Generates a file for each metadata
 * s is the size in bytes of each file chunk to sign but...
 * @param s ...here it gives the number of points u to generate
 */
template<typename ppT>
int initialization(unsigned long long int s)
{
    //TODO exceptions
    G1<ppT> g1 = G1<ppT>::one();
    G2<ppT> g2 = G2<ppT>::one();
    
    Fr<ppT> sk = Fr<ppT>::random_element();
    writeFr<ppT>("sk.bin", sk);

    G2<ppT> pk = sk * g2;
    g2.print();
    cout << "pk = ";
    pk.print();
    writeG2<ppT>("pk.bin", pk);

    Fr<ppT> name = Fr<ppT>::random_element();
    cout << "name = ";
    name.print();
    writeFr<ppT>("name.bin", name);

    G1<ppT> u;
    string u_path = "u.bin";
    ofstream os_u(PATH_DIR + u_path, ios::binary| ios::out);
    for(unsigned long long int j = 0; j < s; j++) {
        u = Fr<ppT>::random_element() * g1;
        cout << "u = ";
        u.print();
        os_u.write((char*) &u.X, sizeof(alt_bn128_Fq));
        os_u.write((char*) &u.Y, sizeof(alt_bn128_Fq));
        os_u.write((char*) &u.Z, sizeof(alt_bn128_Fq));
    }
    
    os_u.close();
    return 0;
}

/**
 * Signs the file by cutting it into chunks of s bytes
 * Creates a file with signatures
 * @param filePath path to the file to sign
 * @param s size of chunks in bytes
 */
template<typename ppT>
int signing(string filePath, unsigned long long int s)
{
    //TODO exceptions
    G1<ppT> g1 = G1<ppT>::one();
    G2<ppT> g2 = G2<ppT>::one();
    
    Fr<ppT> sk = readFr<ppT>("sk.bin");
    Fr<ppT> name = readFr<ppT>("name.bin");
    G1<ppT> u;

    unsigned int m = 0;
    unsigned long long int loop = 0;
    G1<ppT> res = g1;
    G1<ppT> u_m;
    bool stop = false;
    int i = 0;

    ifstream is_to_sign(filePath, ios::binary | ios::in);
    ofstream os_signature("results/signature.bin", ios::binary| ios::out);
    is_to_sign.seekg(0, is_to_sign.beg);

    while (true) { // size / s
        u_m = G1<ppT>::zero();

        for(unsigned long long j = 0; j < s; j++) {
        is_to_sign.read((char*)&m, 1);
        if (is_to_sign.eof()) {
            stop = true;
            break;
        }

        u = readG1Pos<ppT>("u.bin", j);
        u_m = u_m + (Fr<ppT>(m) * u);
        }

        if (stop) break;
        res = sk * (((Fr<ppT>(loop) * name) * g1) + u_m);

        os_signature.write((char*)&res.X, sizeof(alt_bn128_Fq));
        os_signature.write((char*)&res.Y, sizeof(alt_bn128_Fq));
        os_signature.write((char*)&res.Z, sizeof(alt_bn128_Fq));
        loop++;
    }

    os_signature.close();
    is_to_sign.close();
    return 0;
}

/**
 * Creates a challenge, duo of:
 *  - i, random integer as 0 <= i <= file_size/s
 *  - nu, random integer as <= nu
 * Creates a challenge file containing all the challenges
 * @param path path to file we want to challenge (it gives its size)
 * @param c number of challenges to generate
 * @param s size of chunks in bytes
 */
int challenging(string path, unsigned int c, unsigned long long int s) 
{
    //TODO exceptions
    unsigned long long int i = 1;
    unsigned int v = 1;
    ifstream is_file(path, std::ifstream::ate | std::ifstream::binary);
    unsigned long long int taille = is_file.tellg();
    is_file.close();

    ofstream os_challenge("results/challenge.bin", ios::binary| ios::out);  
    for (int j = 0; j < c; j++) {
        i = rand() % (taille/s);
        v = rand() % 500;
        cout << "i = " << to_string(i) << " | v = " << to_string(v) << endl;

        os_challenge.write((char*) &i, sizeof(unsigned long long int));
        os_challenge.write((char*) &v, sizeof(unsigned int));
    }

    os_challenge.close();
    return 0;
}

/**
 * Computes the proof according to the challenge read in the challenge file.
 * Then, the proof is stored in two proof files (for sigma and mu)
 * @param filePath path to the challenge file
 * @param c number of challenge received
 * @param s size of chunks
 */
template<typename ppT>
int proving(string filePath, unsigned int c, unsigned long long int s)
{
    //TODO exceptions
    unsigned long long int i = 1;
    unsigned int v = 1;

    // sigma computation
    G1<ppT> signature;
    G1<ppT> sigma;

    ifstream is_challenge("results/challenge.bin", ios::binary| ios::in);
    for (uint nb = 0; nb < c; nb++) {
        is_challenge.read((char*) &i, sizeof(unsigned long long int));
        is_challenge.read((char*) &v, sizeof(unsigned int));
        signature = readG1Pos<ppT>("signature.bin", i);
        sigma = sigma + (Fr<ppT>(v) * signature);
    }

    is_challenge.seekg(0, is_challenge.beg);
    writeG1<ppT>("sigma.bin", sigma);
    cout << "sigma = ";
    sigma.print();

    // s * mu computation
    unsigned int m = 0;
    unsigned int mu;
    ifstream is_file(filePath, ios::binary | ios::in);
    ofstream os_mu("results/mu.bin", ios::binary | ios::out);
    for (uint j = 0; j < s; j++) {
        mu = 0;
        is_challenge.seekg(0, is_challenge.beg);
        for (uint nb = 0; nb < c; nb++) {
        is_challenge.read((char*) &i, sizeof(unsigned long long int));
        is_challenge.read((char*) &v, sizeof(unsigned int));
        is_file.seekg(i*s+j, is_file.beg);
        is_file.read((char*)&m, 1);
        mu += v * m;
        }

        cout << "mu = " << to_string(mu) << endl;
        os_mu.write((char*)&mu, sizeof(unsigned int));
    } 

    os_mu.close();
    is_file.close();
    is_challenge.close();

    return 0;
}

/**
 * Read both the proof and challenge files in order to verify if the proof is
 * correct according to the challenge generated.
 * @param c number of challenges
 * @param s size of the chunks
 * @return true if the proof is correct, false otherwise
 */
template<typename ppT>
bool verifying(unsigned int c, unsigned long long int s)
{
    G1<ppT> g1 = G1<ppT>::one();
    G2<ppT> g2 = G2<ppT>::one();

    unsigned long long int i = 1;
    unsigned int v = 1;

    // Get the proof
    G1<ppT> sigma = readG1<ppT>("sigma.bin");

    // Get metadata
    Fr<ppT> name = readFr<ppT>("name.bin");
    G2<ppT> pk = readG2<ppT>("pk.bin");

    // First part to computre
    GT<ppT> part1 = ppT::reduced_pairing(sigma, g2);
    
    // Second part
    ifstream is_mu("results/mu.bin", ios::binary | ios::in);
    unsigned int mu = 0;
    G1<ppT> res_u;
    G1<ppT> u;
    for (uint j = 0; j < s; j++) {
        u = readG1Pos<ppT>("u.bin", j);
        is_mu.read((char*)&mu, sizeof(unsigned int));
        res_u = res_u + (Fr<ppT>(mu) * u);
    }

    is_mu.close();

    ifstream is_challenge("results/challenge.bin", ios::binary| ios::in);
    is_challenge.seekg(0, is_challenge.beg);

    G1<ppT> res_h = G1<ppT>::zero();
    for (uint nb = 0; nb < c; nb++) {
        is_challenge.read((char*) &i, sizeof(unsigned long long int));
        is_challenge.read((char*) &v, sizeof(unsigned int));
        res_h = res_h + (Fr<ppT>(v) * (Fr<ppT>(name * i) * g1));
    }

    is_challenge.close();

    G1<ppT> res_right = res_h + res_u;
    GT<ppT> part2 = ppT::reduced_pairing(res_rightt

// In order to compile: g++ -o compact compact.cpp -I ~/.local/include/ -L ~/.local/lib/ -lff -lgmp
/**
 * Full Public Compact Proof of Retrievability protocol
 * Except verification step, every step generates files in order to store
 * all the necessary components to run this protocol.
 * I write all these components in the console in order to use them in the
 * smart contract.
 * TODO: write Json files in the solidity/test folder to use them
 * automatically.
 */
int main(int argc, char *argv[])
{
    //TODO more exceptions...
    if (argc != 4) {
        cerr << "How to use : ./compact [file_path] [chunks_size (=s)] [nb_challenges (=c)]" << endl;
        return -1;
    }
    
    srand((unsigned int)time(NULL));
    alt_bn128_pp::init_public_params();
    
    unsigned long long int s = atoi(argv[2]);
    unsigned int c = atoi(argv[3]);
    
    cout << "Initializing..." << endl;
    initialization<alt_bn128_pp>(s);
    
    cout << "Signing..." << endl;
    signing<alt_bn128_pp>(argv[1], s);

    cout << "Challenging..." << endl;
    challenging(argv[1], c, s);
    
    cout << "Proving..." << endl;
    proving<alt_bn128_pp>(argv[1], c, s);

    cout << "Verifying..." << endl;
    if (verifying<alt_bn128_pp>(c, s)) {
        cout << "Proof is correct!" << endl;
    } else {
        cerr << "Error: proof is NOT correct!" << endl;
    }

    return 0;
}