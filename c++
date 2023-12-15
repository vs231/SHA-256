#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
using namespace std;

//constant values
uint32_t h0 = 0x6a09e667;
uint32_t h1 = 0xbb67ae85;
uint32_t h2 = 0x3c6ef372;
uint32_t h3 = 0xa54ff53a;
uint32_t h4 = 0x510e527f;
uint32_t h5 = 0x9b05688c;
uint32_t h6 = 0x1f83d9ab;
uint32_t h7 = 0x5be0cd19;

uint32_t k[64] = 
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline uint32_t rightRotate(uint32_t value, uint32_t count) 
{
    return (value >> count) | (value << (32 - count));
}

//preprocessing or padding of the given input
string preprocess(const string& initial) 
{
    string binary_rep;//converting into binary
    for (char c : initial) 
    {
        for (int i = 7; i >= 0; --i) 
        {
            binary_rep += ((c >> i) & 1) ? '1' : '0';
        }
    }
    uint64_t org_length = binary_rep.size();
    // cout<<endl<<org_length<<endl;
    string preprocess_message = binary_rep;
    preprocess_message += '1';
    size_t K = 0;
    while (((org_length) + 1 + K + 64) % 512 != 0)//appending 0's till we find a minimum value for K such that the sum is divisible by 512
    {
        preprocess_message += '0';
        K++;
    } 
    string endian;
    for (int i = 63; i >= 0; --i) 
    {
        endian.push_back((org_length & (1ULL << i)) ? '1' : '0');
    }
    preprocess_message+=endian;//appending endian 64-bit representation of original string
    return preprocess_message;
}
    
string process(string pro_message)
{
    string chunk;
    for (int start = 0; start < pro_message.size(); start += 512) 
    {
        chunk = pro_message.substr(start, 512);
        uint32_t wait_schedule[64],x=0;
        for (int i = 0; i < 16; i++) 
        {
            //The initial values in w[0..63] don't matter, so many implementations zero them here copy chunk into first 16 words w[0..15] of the message schedule array
            wait_schedule[i] = stoul(chunk.substr(x,32),nullptr,2);// 2 represents the base for binary
            x+=32;
        }
        for (int i = 16; i < 64; i++)//Extending 16 words to remaining 48 words
        {
            uint32_t s0 = rightRotate(wait_schedule[i - 15],7) ^ rightRotate(wait_schedule[i - 15],18) ^ (wait_schedule[i - 15] >> 3);
            uint32_t s1 = rightRotate(wait_schedule[i - 2],17) ^ rightRotate(wait_schedule[i - 2],19) ^ (wait_schedule[i - 2] >> 10);
            wait_schedule[i] = wait_schedule[i - 16] + s0 + wait_schedule[i - 7] + s1;
        }
        
        uint32_t a = h0;//Initialising the variables with our hash values
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;
        uint32_t f = h5;
        uint32_t g = h6;
        uint32_t h = h7;

        for (int i = 0; i < 64; i++)//Compressing our loop
        {
            uint32_t S1 = rightRotate(e,6) ^ rightRotate(e,11) ^ rightRotate(e,25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h + S1 + ch + k[i] + wait_schedule[i];
            uint32_t S0 = rightRotate(a,2) ^ rightRotate(a,13) ^ rightRotate(a,22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        h0 += a;//Adding them to our current value
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
        h5 += f;
        h6 += g;
        h7 += h;
    }

    stringstream fin;
    fin << hex << setfill('0') << setw(8) << h0;//finding hexadecimal equivalent and appending to get our final hash value
    fin << hex << setfill('0') << setw(8) << h1;
    fin << hex << setfill('0') << setw(8) << h2;
    fin << hex << setfill('0') << setw(8) << h3;
    fin << hex << setfill('0') << setw(8) << h4;
    fin << hex << setfill('0') << setw(8) << h5;
    fin << hex << setfill('0') << setw(8) << h6;
    fin << hex << setfill('0') << setw(8) << h7;

    return fin.str();
}

int main(int argc, char* argv[]) 
{
    string text;
    if(argc <= 1)// single line input
    {
        cout << "Enter Single Line Input(for Multiline create file and process on Command Line Args):\n";
        getline(cin,text);
    }
    else// multi line in form of file inputo
    {
        ifstream ifs(argv[1]);
        if (!ifs.is_open()) 
        {
            cerr << "Error opening file." << endl;
            return 1;
        }
        char c;
        while (ifs.get(c)) 
        {
            text += c;
        }
    }
    cout << "SHA-256 HASH: " << process(preprocess(text)) << endl;
    return 0;
    //THE OUTPUT FOR THE TEXT IN GIVEN LINK IS: 8d202adf39c3e88510a591cbe1cbd96708e3237140060c0768f2577f9d81d3ad
}
