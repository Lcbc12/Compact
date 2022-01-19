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
#define DEBUG false

using namespace libff;
using namespace std;

/**
 * Safe function to open a file given in parameter
 * Throw an exception if the file is eventually not opened
 * @param path file path to open
 * @throw exception if the file can NOT be opened
 * @return an open ifstream of the file path given
 */
ifstream safeOpenIn(string path) {
    ifstream is_file;

    try {
        is_file.open(path, ios::binary | ios::in);
        if(!is_file) {
            throw ifstream::failure("File "+ path +" not found");
        }
    } catch (const ifstream::failure& e) {
        is_file.close();
        throw ifstream::failure(e);
    }

    return is_file;
}

/**
 * Safe function to open a file given in parameter
 * Throw an exception if the file is eventually not opened
 * @param path file path to open
 * @throw exception if the file can NOT be opened
 * @return an open ofstream of the file path given
 */
ofstream safeOpenOut(string path) {
    ofstream os_file;

    try {
        os_file.open(path, ios::binary | ios::out);
        if(!os_file) {
            throw ofstream::failure("Path "+ path +" to file where to write not found");
        }
    } catch (const ofstream::failure& e) {
        os_file.close();
        throw ofstream::failure(e.what());
    }

    return os_file;
}

/**
 * Read data, a Fr point, in the file specifies in path
 * @param path file where the data is written
 * @return Fr point stored in file 
 */
template<typename ppT>
Fr<ppT> readFr(string path)
{
    ifstream is_file;
    Fr<ppT> toReturn;

    try {
        is_file = safeOpenIn(PATH_DIR + path);
        is_file.read((char*) &toReturn, sizeof(Fr<ppT>));
    } catch (const ifstream::failure& e) {
        throw ifstream::failure(e);
    }

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
    alt_bn128_Fq X, Y, Z;
    ifstream is_file;

    try {
        is_file = safeOpenIn(PATH_DIR + path);
        is_file.read((char*) &X, sizeof(alt_bn128_Fq));
        is_file.read((char*) &Y, sizeof(alt_bn128_Fq));
        is_file.read((char*) &Z, sizeof(alt_bn128_Fq));
    } catch (const ifstream::failure& e) {
        throw ifstream::failure(e);
    }

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
    alt_bn128_Fq X, Y, Z;
    ifstream is_file;

    try {
        is_file = safeOpenIn(PATH_DIR + path);
        is_file.seekg(sizeof(G1<ppT>) * position, is_file.beg);
        is_file.read((char*) &X, sizeof(alt_bn128_Fq));
        is_file.read((char*) &Y, sizeof(alt_bn128_Fq));
        is_file.read((char*) &Z, sizeof(alt_bn128_Fq));
    } catch(const ifstream::failure& e) {
        throw ifstream::failure(e);
    }
    
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
    alt_bn128_Fq2 X, Y, Z;
    ifstream is_file;

    try {
        is_file = safeOpenIn(PATH_DIR + path);
        is_file.read((char*) &X, sizeof(alt_bn128_Fq2));
        is_file.read((char*) &Y, sizeof(alt_bn128_Fq2));
        is_file.read((char*) &Z, sizeof(alt_bn128_Fq2));
    } catch(const ifstream::failure& e) {
        throw ifstream::failure(e);
    }

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
    ofstream os_file;

    try {
        os_file = safeOpenOut(PATH_DIR + path);
        os_file.write((char*) &data, sizeof(Fr<ppT>));
    } catch(const ofstream::failure& e) {
        throw ofstream::failure(e);
    }

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
    ofstream os_file;
    alt_bn128_Fq p = data.X;
    
    try {
        os_file = safeOpenOut(PATH_DIR + path);
        os_file.write((char*) &p, sizeof(alt_bn128_Fq));
        p = data.Y;
        os_file.write((char*) &p, sizeof(alt_bn128_Fq));
        p = data.Z;
        os_file.write((char*) &p, sizeof(alt_bn128_Fq));
    } catch(const ofstream::failure& e) {
        throw ofstream::failure(e);
    }

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
    ofstream os_file;
    alt_bn128_Fq2 p = data.X;

    try {
        os_file = safeOpenOut(PATH_DIR + path);
        os_file.write((char*) &p, sizeof(alt_bn128_Fq2));
        p = data.Y;
        os_file.write((char*) &p, sizeof(alt_bn128_Fq2));
        p = data.Z;
        os_file.write((char*) &p, sizeof(alt_bn128_Fq2));
    } catch(const ofstream::failure& e) {
        throw ofstream::failure(e);
    }

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
    Fr<ppT> sk = Fr<ppT>::random_element(),
            name = Fr<ppT>::random_element();
    
    G1<ppT> g1 = G1<ppT>::one(),
            u;

    G2<ppT> g2 = G2<ppT>::one(),
            pk = sk * g2;
    
    ofstream os_u;

    string u_path;

    if (DEBUG) {cout << "sk = ";sk.print();cout << "pk = ";pk.print();cout << "name = ";name.print();}

    try {
        writeFr<ppT>("sk.bin", sk);
        writeG2<ppT>("pk.bin", pk);
        writeFr<ppT>("name.bin", name);

        u_path = "u.bin";
        os_u = safeOpenOut(PATH_DIR + u_path);

        for(unsigned long long int j = 0; j < s; j++) {
            u = Fr<ppT>::random_element() * g1;
            os_u.write((char*) &u.X, sizeof(alt_bn128_Fq));
            os_u.write((char*) &u.Y, sizeof(alt_bn128_Fq));
            os_u.write((char*) &u.Z, sizeof(alt_bn128_Fq));

            if(DEBUG) {cout << "u = ";u.print();}
        }

        os_u.close();
    } catch (const ofstream::failure& os_e) {
        throw ios_base::failure(os_e);
    } catch(const std::exception& e) {
        cerr << e.what() << endl;
        throw std::exception();
    }

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
    Fr<ppT> sk, name;

    G1<ppT> g1 = G1<ppT>::one(),
            res = g1,
            u,
            u_m;

    G2<ppT> g2 = G2<ppT>::one();
    
    int i = 0;
    unsigned int m = 0;
    unsigned long long int loop = 0;

    bool stop = false;

    ifstream is_to_sign;
    ofstream os_signature;

    try {
        sk = readFr<ppT>("sk.bin");
        name = readFr<ppT>("name.bin");
        is_to_sign = safeOpenIn(filePath);
        is_to_sign.seekg(0, is_to_sign.beg);
        os_signature = safeOpenOut("results/signature.bin");

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

            if(DEBUG) {cout << "signature : "; res.print();}
            os_signature.write((char*)&res.X, sizeof(alt_bn128_Fq));
            os_signature.write((char*)&res.Y, sizeof(alt_bn128_Fq));
            os_signature.write((char*)&res.Z, sizeof(alt_bn128_Fq));
            loop++;
        }

    } catch(const std::ios_base::failure& ios_e) {
        throw ios_base::failure(ios_e);
    } catch(const std::exception& e) {
        cerr << e.what() << endl;
        throw std::exception();
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
 * @param size file size we want to challenge
 * @param c number of challenges to generate
 * @param s size of chunks in bytes
 */
int challenging(unsigned long long int size, unsigned int c, unsigned long long int s) 
{
    unsigned long long int i = 1;
    unsigned int nu = 1;
    
    ofstream os_challenge;

    try {
        os_challenge = safeOpenOut("results/challenge.bin");

        for (int j = 0; j < c; j++) {
            i = rand() % (size/s);
            nu = rand() % 500;
            if(DEBUG) {cout << "i = " << to_string(i) << " | nu = " << to_string(nu) << endl;}

            os_challenge.write((char*) &i, sizeof(unsigned long long int));
            os_challenge.write((char*) &nu, sizeof(unsigned int));
        }
    } catch(const ios_base::failure& ios_e) {
        throw ios_base::failure(ios_e);
    } catch(const std::exception& e) {
        cerr << e.what() << endl;
        throw std::exception();
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
    unsigned long long int i = 1;
    unsigned int nu = 1,
                 m = 0,
                 mu;
    G1<ppT> signature, sigma;
    ifstream is_challenge, is_file;
    ofstream os_mu;
    
    try {
        // sigma computation
        is_challenge = safeOpenIn("results/challenge.bin");

        for (uint nb = 0; nb < c; nb++) {
            is_challenge.read((char*) &i, sizeof(unsigned long long int));
            is_challenge.read((char*) &nu, sizeof(unsigned int));
            signature = readG1Pos<ppT>("signature.bin", i);
            sigma = sigma + (Fr<ppT>(nu) * signature);
        }

        is_challenge.seekg(0, is_challenge.beg);
        writeG1<ppT>("sigma.bin", sigma);
        if(DEBUG) {cout << "sigma = ";sigma.print();}

        // s * mu computation
        is_file = safeOpenIn(filePath);
        os_mu = safeOpenOut("results/mu.bin");
        for (uint j = 0; j < s; j++) {
            mu = 0;
            is_challenge.seekg(0, is_challenge.beg);
            for (uint nb = 0; nb < c; nb++) {
                is_challenge.read((char*) &i, sizeof(unsigned long long int));
                is_challenge.read((char*) &nu, sizeof(unsigned int));
                is_file.seekg(i*s+j, is_file.beg);
                is_file.read((char*)&m, 1);
                mu += nu * m;
            }

            if(DEBUG) {cout << "mu = " << to_string(mu) << endl;}
            os_mu.write((char*)&mu, sizeof(unsigned int));
        } 
    } catch(const ios_base::failure& ios_e) {
        throw ios_base::failure(ios_e);
    } catch(const std::exception& e) {
        cerr << e.what() << endl;
        throw std::exception();
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
    Fr<ppT> name;

    G1<ppT> g1 = G1<ppT>::one(),
            u,
            res_u,
            sigma,
            res_h,
            res_right;
            
    G2<ppT> g2 = G2<ppT>::one(),
            pk;

    GT<ppT> part1, part2;

    unsigned long long int i = 1;
    unsigned int nu = 1;
    unsigned int mu = 0;

    ifstream is_challenge, is_mu;

    try {
        // Get the proof
        sigma = readG1<ppT>("sigma.bin");

        // Get metadata
        name = readFr<ppT>("name.bin");
        pk = readG2<ppT>("pk.bin");

        // First part to computre
        part1 = ppT::reduced_pairing(sigma, g2);
        
        // Second part
        is_mu = safeOpenIn("results/mu.bin");
        for (uint j = 0; j < s; j++) {
            u = readG1Pos<ppT>("u.bin", j);
            is_mu.read((char*)&mu, sizeof(unsigned int));
            res_u = res_u + (Fr<ppT>(mu) * u);
        }

        is_mu.close();

        is_challenge = safeOpenIn("results/challenge.bin");
        is_challenge.seekg(0, is_challenge.beg);

        res_h = G1<ppT>::zero();
        for (uint nb = 0; nb < c; nb++) {
            is_challenge.read((char*) &i, sizeof(unsigned long long int));
            is_challenge.read((char*) &nu, sizeof(unsigned int));
            res_h = res_h + (Fr<ppT>(nu) * (Fr<ppT>(name * i) * g1));
        }

        is_challenge.close();

        res_right = res_h + res_u;
        part2 = ppT::reduced_pairing(res_right, pk);

        return part1 == part2;

    } catch(const ios_base::failure& ios_e) {
        throw ios_base::failure(ios_e);
    } catch(std::exception& e) {
        cerr << e.what() << endl;
        throw std::exception();
    }
}


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
    if (argc != 4) {
        cerr << "How to use : ./compact [file_path] [chunks_size (=s)] [nb_challenges (=c)]" << endl;
        return -1;
    }

    unsigned long long int s;
    unsigned int c;
    ifstream is_to_sign;
    unsigned long long int size;

    try {
        // trying to open the file to sign and retrieving its size
        is_to_sign = safeOpenIn(argv[1]);
        is_to_sign.seekg(0, ios::end);
        size = is_to_sign.tellg();
        is_to_sign.close();

        s = stoi(argv[2]);
        c = stoi(argv[3]);

        if (s > size) {
            cerr << "Error: size of chunks (s) must be inferior to file size" << endl;
            throw std::exception();
        }

        srand((unsigned int)time(NULL));
        alt_bn128_pp::init_public_params();
        
        if(DEBUG) cout << "Initializing..." << endl;
        initialization<alt_bn128_pp>(s);
        
        if(DEBUG) cout << "Signing..." << endl;
        signing<alt_bn128_pp>(argv[1], s);

        if(DEBUG) cout << "Challenging..." << endl;
        challenging(size, c, s);
        
        if(DEBUG) cout << "Proving..." << endl;
        proving<alt_bn128_pp>(argv[1], c, s);

        if(DEBUG) cout << "Verifying..." << endl;
        if (verifying<alt_bn128_pp>(c, s)) {
            cout << "Proof is correct!" << endl;
        } else {
            cerr << "Error: proof is NOT correct!" << endl;
        }
    } catch(const ios_base::failure& ios_e) {
        cerr << "IOS Error: " << ios_e.what() << endl;
        return -1;
    } catch(const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return -1;
    }
    return 0;
}