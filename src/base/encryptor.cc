#include "base/encryptor.h"
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <string>
#include <vector>

#include <glog/logging.h>

namespace base {

class MdCtxScoped {
 public:
  MdCtxScoped() {
    EVP_MD_CTX_init(&ctx);   
  }
  ~MdCtxScoped() {
    EVP_MD_CTX_cleanup(&ctx);
  }
  EVP_MD_CTX ctx;

 private:
  DISALLOW_COPY_AND_ASSIGN(MdCtxScoped);
};

void OpenSSLError(const std::string msg) {
  ERR_load_crypto_strings();
  char buf[1024] = {0};
  ERR_error_string_n(ERR_get_error(), buf, sizeof(buf));
  LOG(ERROR) << msg <<  ": ERROR: " << buf;
}

RSA* LoadKeyFromString(const std::string& key, bool is_public_key) {
  RSA* rsa = nullptr;
  if (key.empty()) {
    LOG(ERROR) << "LoadKeyFromString RSA Key is empty.";
    return nullptr;
  }
  BIO* bio = BIO_new_mem_buf(const_cast<char*>(key.data()), key.size());   
  if (bio == nullptr) {
    LOG(ERROR) << "BIO_new_mem_buf return nullptr.";
  }
  if (is_public_key) {
    rsa = PEM_read_bio_RSA_PUBKEY(bio, nullptr, nullptr, nullptr);
  } else {
    rsa = PEM_read_bio_RSAPrivateKey(bio, nullptr, nullptr, nullptr);
  }
  
  BIO_free(bio);
  return rsa;
}

RSA* LoadPublicKeyFromString(const std::string& key) {
  return LoadKeyFromString(key, true);
}

RSA* LoadPrivateKeyFromString(const std::string& key) {
  return LoadKeyFromString(key, false);
}


// RSA private Key
RsaPrivateKey::RsaPrivateKey(RSA* rsa_key) : rsa_key_(rsa_key) {
  DCHECK(rsa_key);
  evp_pkey_ = EVP_PKEY_new();
  DCHECK(evp_pkey_);
  DCHECK(EVP_PKEY_set1_RSA(evp_pkey_, rsa_key_) == 1);
}

RsaPrivateKey::~RsaPrivateKey() {
  if (rsa_key_ != nullptr) {
    RSA_free(rsa_key_);
  }
  if (evp_pkey_ != nullptr) {
    EVP_PKEY_free(evp_pkey_);
  }
}

RsaPrivateKey* RsaPrivateKey::Create(const std::string& pem_key) {
  RSA* rsa_key = LoadPrivateKeyFromString(pem_key);
  return rsa_key == nullptr ? nullptr : new RsaPrivateKey(rsa_key);
}
  
bool RsaPrivateKey::GenerateSignature(const base::StringPiece& message,
                                      std::string* signature) {
  MdCtxScoped ctx_scoped;

  if (EVP_SignInit_ex(&ctx_scoped.ctx, EVP_sha1(), nullptr) != 1) {
    LOG(ERROR) << "EVP_SignInit_ex: ERROR";
    return false;
  }
  if (EVP_SignUpdate(&ctx_scoped.ctx, message.data(), message.size()) != 1) {
    LOG(ERROR) << "EVP_SignUpdate: ERROR";
    return false;
  } 
  
  unsigned int size = EVP_PKEY_size(evp_pkey_);
  signature->resize(size, 0);

  if (EVP_SignFinal(&ctx_scoped.ctx, (unsigned char*)signature->data(), &size, evp_pkey_) != 1) {
    LOG(ERROR) << "EVP_SignFinal: ERROR";
    return false;
  }

  return true;
}

// Public key
RsaPublicKey::RsaPublicKey(RSA* rsa_key) : rsa_key_(rsa_key) {
  DCHECK(rsa_key);
  evp_pkey_ = EVP_PKEY_new();
  DCHECK(evp_pkey_);
  DCHECK(EVP_PKEY_set1_RSA(evp_pkey_, rsa_key_) == 1);
}

RsaPublicKey::~RsaPublicKey() {
  if (rsa_key_ != nullptr) { 
    RSA_free(rsa_key_);
  } 
  if (evp_pkey_ != nullptr) { 
    EVP_PKEY_free(evp_pkey_);
  } 
}

RsaPublicKey* RsaPublicKey::Create(const std::string& pem_key) { 
  RSA* rsa_key = LoadPublicKeyFromString(pem_key);
  return rsa_key == nullptr ? nullptr : new RsaPublicKey(rsa_key);
}

bool RsaPublicKey::VerifySignature(const base::StringPiece& message,
                                   const base::StringPiece& signature) {
  MdCtxScoped ctx_scoped;
  if (EVP_VerifyInit_ex(&ctx_scoped.ctx, EVP_sha1(), nullptr) != 1) {
    LOG(ERROR) << "EVP_VerifyInit_ex: ";
    return false;
  }

  if (EVP_VerifyUpdate(&ctx_scoped.ctx, message.data(), message.size()) != 1) {
    LOG(ERROR) << "EVP_VerifyUpdate:";
    return false;
  }

  if (EVP_VerifyFinal(&ctx_scoped.ctx, (unsigned char*)signature.data(), 
                      signature.size(), evp_pkey_) != 1) {
    OpenSSLError("EVP_VerifyFinal: ");
    return false;
  }
  return true;
}


} // namespace base
