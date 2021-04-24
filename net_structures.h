#ifndef _CUSTOM_FAKELAG_NET_STRUCTURES_H_
#define _CUSTOM_FAKELAG_NET_STRUCTURES_H_
#include <netadr.h>

typedef struct dumb_netadr_s {
  netadrtype_t	type;
  unsigned char	ip[4];
  unsigned short	port;
} dumb_netadr_t;


// I don't want to pull in all the bf_read code, since it has compile issues, and we ignore it.
struct fake_bf_read {
  const unsigned char*	m_pData;
  int						m_nDataBytes;
  int						m_nDataBits;
  int				m_iCurBit;
  //private:	
  bool			m_bOverflow;
  bool			m_bAssertOnOverflow;
  const char		*m_pDebugName;

  // These are probably CBitRead on hl2sdk-l4d2. 
  // Could be different amounts of data here on other SDK versions
  int m_BitRead0;
  int m_BitRead1;
  int m_BitRead2;
};

// Copied from hl2sdk so we don't have to include all that junk
typedef struct _netpacket_s 
{
  // !!!
  // This netadr_t is INCORRECT!!!!!
  // can I use a custom one instead???
  // !!!!!!!
  dumb_netadr_t		from;		// sender IP
  int				source;		// received source 
  double			received;	// received time
  unsigned char	*data;		// pointer to raw packet data
  struct fake_bf_read			message;	// easy bitbuf data access
  int				size;		// size in bytes
  int				wiresize;   // size in bytes before decompression
  bool			stream;		// was send as stream
  struct _netpacket_s *pNext;	// for internal use, should be NULL in public

  _netpacket_s(const _netpacket_s& src) :
    from(src.from),
    source(src.source),
    received(src.received),
    message(src.message),
    size(src.size),
    wiresize(src.wiresize),
    stream(src.stream),
    pNext(NULL)
  {
	  data = new unsigned char[src.size];
	  // TODO: Pick a different memcpy?
	  memcpy(data, src.data, src.size);
  }
} _netpacket_t;

#endif // _CUSTOM_FAKELAG_NET_STRUCTURES_H_
