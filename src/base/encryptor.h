#ifndef HANDLERS_RSA_KEY_H_
#define HANDLERS_RSA_KEY_H_

#include <string>

#include "base/macros.h"
#include "base/stringpiece.h"

struct rsa_st;
typedef struct rsa_st RSA;

struct evp_pkey_st;
typedef struct evp_pkey_st EVP_PKEY;

namespace base {

const std::string kAliMobilePublicKey =
"-----BEGIN PUBLIC KEY-----\n"
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCnxj/9qwVfgoUh/y2W89L6BkRA\n"
"FljhNhgPdyPuBV64bfQNN1PjbCzkIM6qRdKBoLPXmKKMiFYnkd6rAoprih3/PrQE\n"
"B/VsW8OoM8fxn67UDYuyBTqA23MML9q1+ilIZwBC2AQ2UBVOrFXfFl75p6/B5Ksi\n"
"NG9zpgmLCUYuLkxpLQIDAQAB\n"
"-----END PUBLIC KEY-----";


const std::string kAliMobilePrivateKey =
"-----BEGIN PRIVATE KEY-----\n"
"MIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBANm9ifUE0kKKMD1r\n"
"PqJ6Swv1rqkj+1cTPOaU8msS6mvoTATNy+GKPX3gr7zbxkBmyqUWKl0rX2sJsh1F\n"
"+89h4v9SFsAU21vVAkWm2mhPKmv0C0NwP0IMlVUqQQm6A/KHDqdBGx2Agfy0xBsw\n"
"sshCX5+ZTz6EN/1Nm1VsPTtZdzwDAgMBAAECgYEAmJdj3voJ/+en9uL5ehQKE8+R\n"
"H6e/tD4fJ9iqKq+O6SBaZeNzRhQDE/wqLEg4n1lqGld2SOTkcbbRxAIPfj4zPT+Z\n"
"pj0hzTLH1La508hp2maHKYJB3j/U2ybP5e0pq6b/gD4kzSgWEnsLnoEnTOFMpxgb\n"
"33DNJ2n3CnAVMMxzckECQQD3e8bnVjHQu4rHi7VR+8fUAdUnCCuEh4ADynqUn0xw\n"
"hz/5YDyW2Ryt3L7BEPkj52PnKn0PjKeD4wIu5zpQsE3tAkEA4Tu85ZgDa2NzIe9w\n"
"C2uJl0974OWSZbOEpF9jxl7vv963x/J3P4ZkPBFEuzoZoM1bMg9mGzFrr/Ro9ecC\n"
"w6LzrwJAEWiRi0BS7L3kezjmnke5vL4MQlyewwnNBXc+vCmgdOHurBoK6QmIrOo0\n"
"ONx6yf3C10Xz2h4owmw7gRUe0wIz2QJAWow0jazGOyb5AzyKM90grHbk224Po9k+\n"
"ywIaT4adJeq4waZlcGFr7IYYtphN80p7ji2IpAgDPfKpO6qKfvIEuwJAWeWP5rOJ\n"
"qYLa5rf8BHVQwj1pu9XDkUsMmQ1tp2LmyYIrdO9MWKloOnDrQuAkdPxGZrPmlhXy\n"
"2s5+NrbHD5S8EQ==\n"
"-----END PRIVATE KEY-----";


class PrivateKey {
 public:
  virtual ~PrivateKey() {}  
  virtual bool GenerateSignature(const base::StringPiece& message,
                                 std::string* signature) = 0;
};

class PublicKey {
 public:
  virtual ~PublicKey() {}
  virtual bool VerifySignature(const base::StringPiece& message,
                               const base::StringPiece& signature) = 0;
};

class RsaPrivateKey : public PrivateKey {
 public:
  ~RsaPrivateKey();

  static RsaPrivateKey* Create(const std::string& key);
  virtual bool GenerateSignature(const base::StringPiece& message,
                                 std::string* signaure) override;

 private:
  explicit RsaPrivateKey(RSA* rsa_key);

  RSA* rsa_key_;
  EVP_PKEY* evp_pkey_;

  DISALLOW_COPY_AND_ASSIGN(RsaPrivateKey);
};

class RsaPublicKey : public PublicKey {
 public:
  ~RsaPublicKey();

  static RsaPublicKey* Create(const std::string& key);
  virtual bool VerifySignature(const base::StringPiece& message,
                               const base::StringPiece& signature) override;
 private:
  explicit RsaPublicKey(RSA* rsa_key);

  RSA* rsa_key_;
  EVP_PKEY* evp_pkey_;

  DISALLOW_COPY_AND_ASSIGN(RsaPublicKey);
};



/////////////////////////////////// Ali mobile rsa keys
//
// Singleton Pattern
//
class AliMobileRsaKey {
 public:

  static AliMobileRsaKey* GetInstance() {
    static AliMobileRsaKey instance;
    return &instance;
  }

  RsaPrivateKey* GetPrivateKey() {
    return private_key_;
  }

  RsaPublicKey* GetPublicKey() {
    return public_key_;
  }

 private:

  RsaPrivateKey* private_key_;
  RsaPublicKey* public_key_;

  AliMobileRsaKey() {
    private_key_ = RsaPrivateKey::Create(kAliMobilePrivateKey);
    public_key_ = RsaPublicKey::Create(kAliMobilePublicKey);
  }

  DISALLOW_COPY_AND_ASSIGN(AliMobileRsaKey);
};

} // namespace handlers;
#endif // HANDLERS_RSA_KEY_H_
