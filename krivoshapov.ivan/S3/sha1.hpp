#ifndef KRIVOSHAPOV_SHA1_HPP
#define KRIVOSHAPOV_SHA1_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

namespace krivoshapov
{
  namespace detail
  {
    inline uint32_t rotl(uint32_t v, int c)
    {
      return (v << c) | (v >> (32 - c));
    }
  }

  inline void sha1(const unsigned char *data, size_t len, unsigned char out[20])
  {
    uint32_t h0 = 0x67452301u;
    uint32_t h1 = 0xEFCDAB89u;
    uint32_t h2 = 0x98BADCFEu;
    uint32_t h3 = 0x10325476u;
    uint32_t h4 = 0xC3D2E1F0u;

    uint64_t bitLen = static_cast<uint64_t>(len) * 8u;
    size_t total = len + 1;
    while (total % 64 != 56)
    {
      ++total;
    }
    total += 8;

    for (size_t off = 0; off < total; off += 64)
    {
      unsigned char block[64];
      for (size_t i = 0; i < 64; ++i)
      {
        size_t pos = off + i;
        unsigned char b = 0;
        if (pos < len)
        {
          b = data[pos];
        }
        else if (pos == len)
        {
          b = 0x80;
        }
        else if (pos >= total - 8)
        {
          int shift = static_cast<int>((total - 1 - pos) * 8);
          b = static_cast<unsigned char>((bitLen >> shift) & 0xFF);
        }
        block[i] = b;
      }

      uint32_t w[80];
      for (int i = 0; i < 16; ++i)
      {
        w[i] = (static_cast<uint32_t>(block[i * 4]) << 24)
             | (static_cast<uint32_t>(block[i * 4 + 1]) << 16)
             | (static_cast<uint32_t>(block[i * 4 + 2]) << 8)
             | static_cast<uint32_t>(block[i * 4 + 3]);
      }
      for (int i = 16; i < 80; ++i)
      {
        w[i] = detail::rotl(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
      }

      uint32_t a = h0;
      uint32_t b = h1;
      uint32_t c = h2;
      uint32_t d = h3;
      uint32_t e = h4;

      for (int i = 0; i < 80; ++i)
      {
        uint32_t f = 0;
        uint32_t k = 0;
        if (i < 20)
        {
          f = (b & c) | ((~b) & d);
          k = 0x5A827999u;
        }
        else if (i < 40)
        {
          f = b ^ c ^ d;
          k = 0x6ED9EBA1u;
        }
        else if (i < 60)
        {
          f = (b & c) | (b & d) | (c & d);
          k = 0x8F1BBCDCu;
        }
        else
        {
          f = b ^ c ^ d;
          k = 0xCA62C1D6u;
        }
        uint32_t tmp = detail::rotl(a, 5) + f + e + k + w[i];
        e = d;
        d = c;
        c = detail::rotl(b, 30);
        b = a;
        a = tmp;
      }

      h0 += a;
      h1 += b;
      h2 += c;
      h3 += d;
      h4 += e;
    }

    uint32_t hs[5] = {h0, h1, h2, h3, h4};
    for (int i = 0; i < 5; ++i)
    {
      out[i * 4] = static_cast<unsigned char>((hs[i] >> 24) & 0xFF);
      out[i * 4 + 1] = static_cast<unsigned char>((hs[i] >> 16) & 0xFF);
      out[i * 4 + 2] = static_cast<unsigned char>((hs[i] >> 8) & 0xFF);
      out[i * 4 + 3] = static_cast<unsigned char>(hs[i] & 0xFF);
    }
  }

  inline size_t sha1Hash(const unsigned char *data, size_t len)
  {
    unsigned char d[20];
    sha1(data, len, d);
    size_t r = 0;
    for (int i = 0; i < 8; ++i)
    {
      r = (r << 8) | d[i];
    }
    return r;
  }

  struct StringHash
  {
    size_t operator()(const std::string &s) const
    {
      return sha1Hash(reinterpret_cast<const unsigned char *>(s.data()), s.size());
    }
  };

  struct StringEqual
  {
    bool operator()(const std::string &a, const std::string &b) const
    {
      return a == b;
    }
  };

  using VertexPair = std::pair<std::string, std::string>;

  struct PairHash
  {
    size_t operator()(const VertexPair &p) const
    {
      std::string buf = p.first;
      buf.push_back('\0');
      buf += p.second;
      return sha1Hash(reinterpret_cast<const unsigned char *>(buf.data()), buf.size());
    }
  };

  struct PairEqual
  {
    bool operator()(const VertexPair &a, const VertexPair &b) const
    {
      return a.first == b.first && a.second == b.second;
    }
  };
}

#endif
