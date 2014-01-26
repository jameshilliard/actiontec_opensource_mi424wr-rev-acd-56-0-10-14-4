WPA_DIR=$(WLAN_BASE)/wlan_driver/src/core/WPA
OPENSSL_DIR=$(WPA_DIR)/openssl
CRYPTO_DIR=$(OPENSSL_DIR)/crypto

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/rc4/,rc4_enc.o rc4_skey.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/asn1/,a_object.o a_bitstr.o a_utctm.o \
  a_gentm.o a_time.o a_int.o a_octet.o a_null.o a_print.o a_type.o a_set.o \
  a_dup.o a_d2i_fp.o a_i2d_fp.o a_bmp.o a_enum.o a_vis.o a_utf8.o a_sign.o \
  a_digest.o a_verify.o a_mbstr.o a_strex.o x_algor.o x_val.o x_pubkey.o \
  x_sig.o x_req.o x_attrib.o x_name.o x_cinf.o x_x509.o x_x509a.o x_crl.o \
  x_info.o x_spki.o nsseq.o d2i_r_pr.o i2d_r_pr.o d2i_r_pu.o i2d_r_pu.o \
  d2i_s_pr.o i2d_s_pr.o d2i_s_pu.o i2d_s_pu.o d2i_pu.o d2i_pr.o i2d_pu.o \
  i2d_pr.o t_req.o t_x509.o t_x509a.o t_crl.o t_pkey.o t_spki.o t_bitst.o \
  p7_i_s.o p7_signi.o p7_signd.o p7_recip.o p7_enc_c.o p7_evp.o p7_dgst.o \
  p7_s_e.o p7_enc.o p7_lib.o f_int.o f_string.o i2d_dhp.o i2d_dsap.o d2i_dhp.o \
  d2i_dsap.o n_pkey.o f_enum.o a_hdr.o x_pkey.o a_bool.o x_exten.o asn1_par.o \
  asn1_lib.o asn1_err.o a_meth.o a_bytes.o a_strnid.o evp_asn1.o asn_pack.o \
  p5_pbe.o p5_pbev2.o p8_pkey.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/md5/,md5_dgst.o md5_one.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/sha/,sha_dgst.o sha1dgst.o sha_one.o \
  sha1_one.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/ripemd/,rmd_dgst.o rmd_one.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/rsa/,rsa_eay.o rsa_gen.o rsa_lib.o \
  rsa_sign.o rsa_saos.o rsa_err.o rsa_pk1.o rsa_ssl.o rsa_none.o rsa_oaep.o \
  rsa_chk.o rsa_null.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/bn/,bn_add.o bn_div.o bn_exp.o \
  bn_lib.o bn_ctx.o bn_mul.o bn_print.o bn_rand.o bn_shift.o bn_word.o \
  bn_blind.o bn_gcd.o bn_prime.o bn_err.o bn_sqr.o $(BN_ASM) bn_recp.o \
  bn_mont.o bn_mpi.o bn_exp2.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/dsa/,dsa_gen.o dsa_key.o dsa_lib.o \
  dsa_asn1.o dsa_vrf.o dsa_sign.o dsa_err.o dsa_ossl.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/dso/,dso_dl.o dso_err.o dso_lib.o \
  dso_null.o dso_openssl.o dso_win32.o dso_vms.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/buffer/,buffer.o buf_err.o) 

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/bio/,bio_lib.o bio_cb.o bio_err.o \
	bss_mem.o bss_null.o bss_fd.o bss_file.o bss_sock.o bss_conn.o \
	bf_null.o bf_buff.o b_print.o b_dump.o b_sock.o bss_acpt.o bf_nbio.o \
	bss_log.o bss_bio.o) 

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/stack/,stack.o) 

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/lhash/,lhash.o lh_stats.o) 

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/rand/,md_rand.o randfile.o rand_lib.o \
  rand_err.o rand_egd.o rand_win.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/err/,err.o err_all.o err_prn.o) 

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/dh/,dh_gen.o dh_key.o dh_lib.o \
  dh_check.o dh_err.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/objects/,o_names.o obj_dat.o obj_lib.o \
  obj_err.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/evp/,encode.o digest.o evp_enc.o \
  evp_key.o e_des.o e_bf.o e_idea.o e_des3.o e_rc4.o names.o e_xcbc_d.o \
  e_rc2.o e_cast.o e_rc5.o m_null.o m_md2.o m_md4.o m_md5.o m_sha.o m_sha1.o \
  m_dss.o m_dss1.o m_mdc2.o m_ripemd.o p_open.o p_seal.o p_sign.o p_verify.o \
  p_lib.o p_enc.o p_dec.o bio_md.o bio_b64.o bio_enc.o evp_err.o e_null.o \
  c_all.o c_allc.o c_alld.o evp_lib.o bio_ok.o evp_pkey.o evp_pbe.o p5_crpt.o \
  p5_crpt2.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/pem/,pem_sign.o pem_seal.o pem_info.o \
  pem_lib.o pem_all.o pem_err.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/x509/,x509_def.o x509_d2.o x509_r2x.o \
  x509_cmp.o x509_obj.o x509_req.o x509spki.o x509_vfy.o x509_set.o x509rset.o \
  x509_err.o x509name.o x509_v3.o x509_ext.o x509_att.o x509type.o x509_lu.o \
  x_all.o x509_txt.o x509_trs.o by_file.o by_dir.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/x509v3/,v3_bcons.o v3_bitst.o \
  v3_conf.o v3_extku.o v3_ia5.o v3_lib.o v3_prn.o v3_utl.o v3err.o v3_genn.o \
  v3_alt.o v3_skey.o v3_akey.o v3_pku.o v3_int.o v3_enum.o v3_sxnet.o \
  v3_cpols.o v3_crld.o v3_purp.o v3_info.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/conf/,conf_err.o conf_lib.o conf_api.o \
  conf_def.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/txt_db/,txt_db.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/pkcs7/,pk7_lib.o pkcs7err.o pk7_doit.o \
  pk7_smime.o pk7_attr.o pk7_mime.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/pkcs12/,p12_add.o p12_attr.o \
  p12_bags.o p12_crpt.o p12_crt.o p12_decr.o p12_init.o p12_key.o p12_kiss.o \
  p12_lib.o p12_mac.o p12_mutl.o p12_sbag.o p12_utl.o p12_npas.o pk12err.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/comp/,comp_lib.o comp_err.o c_rle.o \
  c_zlib.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/des/,set_key.o  ecb_enc.o  cbc_enc.o \
	ecb3_enc.o cfb64enc.o cfb64ede.o cfb_enc.o  ofb64ede.o enc_read.o \
	enc_writ.o ofb64enc.o ofb_enc.o  str2key.o  pcbc_enc.o qud_cksm.o \
	rand_key.o read2pwd.o fcrypt.o xcbc_enc.o read_pwd.o rpc_enc.o \
	cbc_cksm.o ede_cbcm_enc.o des_enc.o fcrypt_b.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/md4/,md4_dgst.o md4_one.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/hmac/,hmac.o)

CRYPTO_FILES+=$(addprefix $(CRYPTO_DIR)/,cryptlib.o mem.o mem_dbg.o cversion.o \
  ex_data.o tmdiff.o cpt_err.o ebcdic.o uid.o)

SSL_FILES+=$(addprefix $(OPENSSL_DIR)/ssl/,s2_meth.o s2_srvr.o s2_clnt.o \
  s2_lib.o s2_enc.o s2_pkt.o s3_meth.o s3_srvr.o s3_clnt.o s3_lib.o s3_enc.o \
  s3_pkt.o s3_both.o s23_meth.o s23_srvr.o s23_clnt.o s23_lib.o s23_pkt.o \
  t1_meth.o t1_srvr.o t1_clnt.o t1_lib.o t1_enc.o ssl_lib.o ssl_err2.o \
  ssl_cert.o ssl_sess.o ssl_ciph.o ssl_stat.o ssl_rsa.o ssl_asn1.o ssl_txt.o \
  ssl_algs.o bio_ssl.o ssl_err.o)
