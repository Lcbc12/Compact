/**
 *****************************************************************************
 * @author     Lucas GICQUEL
 *****************************************************************************/

#include <libff/algebra/curves/edwards/edwards_pp.hpp>
#include <libff/common/profiling.hpp>
#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libff/algebra/curves/mnt/mnt4/mnt4_pp.hpp>
#include <libff/algebra/curves/mnt/mnt6/mnt6_pp.hpp>

#include <chrono>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

#define PATH_DIR "results/"
#define DEBUG false

using namespace std::chrono;
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
 * Get the size of the file given in parameter
 * @param path the path to the file we want the size
 * @return the size of the file
 */
unsigned long long int getSize(string path) {
    ifstream is_file;
    unsigned long long int size;

    try {
        is_file = safeOpenIn(path);
        is_file.seekg(0, ios::end);
        size = is_file.tellg();
        is_file.close();
    } catch(const ifstream::failure& e) {
        is_file.close();
        throw ifstream::failure(e);
    }

    return size;
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


// To compile: g++ -o compact compact.cpp -I ~/.local/include/ -L ~/.local/lib/ -lff -lgmp -ljsoncpp
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
    if (argc != 6) {
        cerr << "How to use : ./compact <file_path> <chunks_size_min> (=s_min) <chunks_size_max> (=s_max) <interval> <nb_challenges> (=c)" << endl;
        return -1;
    }

    uint nb_valid = 0;
    unsigned int c;
    ifstream is_to_sign;
    ifstream is_file;
    unsigned long long int s_min,
                           s_max,
                           interval,
                           size,
                           init_size,
                           proof_size,
                           challenge_size,
                           signature_size;

    try {
        // trying to open the file to sign and retrieving its size
        is_to_sign = safeOpenIn(argv[1]);
        is_to_sign.seekg(0, ios::end);
        size = is_to_sign.tellg();
        is_to_sign.close();

        // write times in a CSV file
        FILE *os_times = fopen("results/benchmarks.csv", "w");
        fprintf(os_times, "algo,size,s,clients,exec_time,init_time,signing_time,challenging_time,proving_time,verif_time,init_size,signing_size,challenging_size,proving_size\n");

        s_min = stoi(argv[2]);
        s_max = stoi(argv[3]);
        interval = stoi(argv[4]);
        c = stoi(argv[5]);

        if (s_max > size) {
            cerr << "Error: size of chunks (s_max) must be inferior to file size" << endl;
            throw std::exception();
        }

        srand((unsigned int)time(NULL));
        alt_bn128_pp::init_public_params();

        for(unsigned long long int i = s_min; i <= s_max; i+=interval) {
            
            if(DEBUG) cout << "Loop n°" << to_string(i) << endl;

            if(DEBUG) cout << "Initializing..." << endl;
            auto time_start = high_resolution_clock::now();
            initialization<alt_bn128_pp>(i);
            
            if(DEBUG) cout << "Signing..." << endl;
            auto time_sign = high_resolution_clock::now();
            signing<alt_bn128_pp>(argv[1], i);

            if(DEBUG) cout << "Challenging..." << endl;
            auto time_challenge = high_resolution_clock::now();
            challenging(size, c, i);
            
            if(DEBUG) cout << "Proving..." << endl;
            auto time_proof = high_resolution_clock::now();
            proving<alt_bn128_pp>(argv[1], c, i);

            if(DEBUG) cout << "Verifying..." << endl;
            auto time_verify = high_resolution_clock::now();
            if (verifying<alt_bn128_pp>(c, i)) {
                cout << "Proof is correct!" << endl;
                nb_valid++;
            } else {
                cerr << "Error: proof is NOT correct!" << endl;
            }

            auto time_stop = high_resolution_clock::now();

            init_size = getSize("results/name.bin");
            init_size += getSize("results/u.bin");
            init_size += getSize("results/pk.bin");
            init_size += getSize("results/sk.bin");

            signature_size = getSize("results/signature.bin");

            challenge_size = getSize("results/challenge.bin");

            proof_size = getSize("results/sigma.bin");
            proof_size += getSize("results/mu.bin");

            unsigned int exec_time = duration_cast<seconds>(time_stop - time_start).count();
            fprintf(
                os_times,
                "compact,%lli,%lli,1,%li,%li,%li,%li,%li,%li,%lli,%lli,%lli,%lli\n",
                size, i,
                duration_cast<seconds>(time_stop - time_start).count(),
                duration_cast<seconds>(time_sign - time_start).count(),
                duration_cast<seconds>(time_challenge - time_sign).count(),
                duration_cast<seconds>(time_proof - time_challenge).count(),
                duration_cast<seconds>(time_verify - time_proof).count(),
                duration_cast<seconds>(time_stop - time_verify).count(),
                init_size, signature_size, challenge_size, proof_size
            );

            if (DEBUG) {
                cout << "Execution time: " << duration_cast<seconds>(time_stop - time_start).count() << " seconds" << endl;
                cout << "Initialization step: " << duration_cast<seconds>(time_start - time_sign).count() << " seconds" << endl;
                cout << "Signing step: " << duration_cast<seconds>(time_challenge - time_sign).count() << " seconds" << endl;
                cout << "Challenging step: " << duration_cast<seconds>(time_proof - time_challenge).count() << " seconds" << endl;
                cout << "Proving step: " << duration_cast<seconds>(time_verify - time_proof).count() << " seconds" << endl;
                cout << "Verifying step: " << duration_cast<seconds>(time_stop - time_verify).count() << " seconds" << endl;
            }

            ifstream profile("profile.json");
            ofstream lala("profile2.json");

            Json::Reader reader;
            Json::Value obj, obj2;
            reader.parse(profile, obj);

            obj2.Value(2);
            obj2.append(obj2.Value("lala"));
            Json::StyledWriter writer;
            lala << writer.write(obj2);
            lala.close();

            cout << "Last Name: " << obj["lastname"].asString() << endl;
            cout << "First Name: " << obj["firstname"].asString() << endl;
        }

        cout << to_string(nb_valid) << "/" << to_string((s_max - s_min + 1)/interval) << " proof valid" << endl;
        fclose(os_times);

    } catch(const ios_base::failure& ios_e) {
        cerr << "IOS Error: " << ios_e.what() << endl;
        return -1;
    } catch(const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return -1;
    }

    return 0;
}