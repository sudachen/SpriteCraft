
#if !defined __60EDC02C_AAD6_415D_B287_0490666C10E9__
#define __60EDC02C_AAD6_415D_B287_0490666C10E9__

#if !defined _WITHOUT_TFFCLS
#include "./_specific.h"
#else
#include "./detect_compiler.h"
#endif

namespace teggo
{

  class MD5_Hash {
  public:
    MD5_Hash();
    void Update(void const *input, unsigned long input_length);
    void Finalize(void *outDigest);

  private:
    u32_t state_[4];
    u32_t count_[2];  // number of *bits*, mod 2^64
    byte_t buffer_[64];
    byte_t digest_[16];
    byte_t finalized_;
    void Init();
    void Transform(byte_t *buffer);
    void Encode(byte_t *dest, u32_t *src, u32_t length);
    void Decode(u32_t *dest, byte_t *src, u32_t length);

    // FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.

    static u32_t RotateLeft (u32_t x, u32_t n)  { return (x << n) | (x >> (32-n)); }
    // F, G, H and I are basic MD5_Hash functions.
    static u32_t  F(u32_t x, u32_t y, u32_t z)  { return (x & y) | (~x & z); }
    static u32_t  G(u32_t x, u32_t y, u32_t z)  { return (x & z) | (y & ~z); }
    static u32_t  H(u32_t x, u32_t y, u32_t z)  { return x ^ y ^ z; }
    static u32_t  I(u32_t x, u32_t y, u32_t z)  { return y ^ (x | ~z); }

    static void   FF(u32_t& a, u32_t b, u32_t c, u32_t d, u32_t x, u32_t s, u32_t ac)
    {
      a += F(b, c, d) + x + ac;
      a = RotateLeft (a, s) +b;
    }

    static void   GG(u32_t& a, u32_t b, u32_t c, u32_t d, u32_t x, u32_t s, u32_t ac)
    {
      a += G(b, c, d) + x + ac;
      a = RotateLeft (a, s) +b;
    }

    static void   HH(u32_t& a, u32_t b, u32_t c, u32_t d, u32_t x, u32_t s, u32_t ac)
    {
      a += H(b, c, d) + x + ac;
      a = RotateLeft (a, s) +b;
    }

    static void   II(u32_t& a, u32_t b, u32_t c, u32_t d, u32_t x, u32_t s, u32_t ac)
    {
      a += I(b, c, d) + x + ac;
      a = RotateLeft (a, s) +b;
    }

    enum {
      S11 = 7,
      S12 = 12,
      S13 = 17,
      S14 = 22,
      S21 = 5,
      S22 = 9,
      S23 = 14,
      S24 = 20,
      S31 = 4,
      S32 = 11,
      S33 = 16,
      S34 = 23,
      S41 = 6,
      S42 = 10,
      S43 = 15,
      S44 = 21
    };

  };


  inline MD5_Hash::MD5_Hash()
  {
    Init();
  }

  CXX_FAKE_INLINE
  void MD5_Hash::Update(void const *_input, unsigned long input_length)
  {
    u32_t input_index, buffer_index;
    u32_t buffer_space;                // how much space is left in buffer
    byte_t *input=(byte_t *)_input;

    if (finalized_)// we can't update!
    {
      return;
    }

    // Compute number of bytes mod 64
    buffer_index = (unsigned int)((count_[0] >> 3) & 0x3F);

    // Update number of bits
    if (  (count_[0] += ((u32_t) input_length << 3))<((u32_t) input_length << 3) )
      count_[1]++;

    count_[1] += ((u32_t)input_length >> 29);


    buffer_space = 64 - buffer_index;  // how much space is left in buffer

    // Transform as many times as possible.
    if (input_length >= buffer_space) // ie. we have enough to fill the buffer
    {
      // fill the rest of the buffer and transform
      memcpy (buffer_ + buffer_index, input, buffer_space);
      Transform (buffer_);

      // now, transform each 64-byte piece of the input, bypassing the buffer
      for (input_index = buffer_space; input_index + 63 < input_length; input_index += 64)
        Transform (input+input_index);

      buffer_index = 0;  // so we can buffer remaining
    }
    else
      input_index=0;     // so we can buffer the whole input


    // and here we do the buffering:
    memcpy(buffer_+buffer_index, input+input_index, input_length-input_index);
  }

  // AlgMD5 finalization. Writing the message digest and zeroizing the context.
  CXX_FAKE_INLINE
  void MD5_Hash::Finalize (void *outDigest)
  {
    unsigned char bits[8];
    unsigned int index, padLen;
    byte_t PADDING[64]={
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    if (finalized_) {
      if ( outDigest )
        memcpy(outDigest,digest_,16);
      return;
    }

    // Save number of bits
    Encode (bits, count_, 8);
    // Pad out to 56 mod 64.
    index = (u32_t) ((count_[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    Update (PADDING, padLen);
    // Append length (before padding)
    Update (bits, 8);
    // Store state in digest
    Encode (digest_, state_, 16);

    if ( outDigest )
      memcpy(outDigest,digest_,16);

    // Zero sensitive information
    memset (buffer_, 0, sizeof(*buffer_));
    finalized_=1;
  }

  // Private methods

  inline void MD5_Hash::Init()
  {
    finalized_=0;  // start

    // zero count=0
    count_[0] = 0;
    count_[1] = 0;

    // load initialization constants.
    state_[0] = 0x67452301;
    state_[1] = 0xefcdab89;
    state_[2] = 0x98badcfe;
    state_[3] = 0x10325476;
  }

  // Constants for AlgMD5 Transform

  // AlgMD5 basic transformation.
  CXX_FAKE_INLINE
  void MD5_Hash::Transform(byte_t block[64])
  {

    u32_t a = state_[0], b = state_[1], c = state_[2], d = state_[3], x[16];

    Decode (x, block, 64);

    /* Round 1 */
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

   /* Round 2 */
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;

    // clear sensitive information.
    memset ( (byte_t *) x, 0, sizeof(x));

  }

  // Encodes input (4 byte) into output (1 byte)
  CXX_FAKE_INLINE
  void MD5_Hash::Encode(byte_t *output, u32_t *input, u32_t len)
  {
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
    {
      output[j]   = (byte_t)  (input[i] & 0xff);
      output[j+1] = (byte_t) ((input[i] >> 8) & 0xff);
      output[j+2] = (byte_t) ((input[i] >> 16) & 0xff);
      output[j+3] = (byte_t) ((input[i] >> 24) & 0xff);
    }
  }

  // Decodes input (1 byte) into output (4 byte)
  CXX_FAKE_INLINE
  void MD5_Hash::Decode (u32_t *output, byte_t *input, u32_t len)
  {
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
      output[i] = ((u32_t)input[j]) | (((u32_t)input[j+1]) << 8) | (((u32_t)input[j+2]) << 16) |
      (((u32_t)input[j+3]) << 24);
  }

} // namespace

#endif // __60EDC02C_AAD6_415D_B287_0490666C10E9__
