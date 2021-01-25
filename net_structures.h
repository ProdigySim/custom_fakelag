#ifndef _CUSTOM_FAKELAG_NET_STRUCTURES_H_
#define _CUSTOM_FAKELAG_NET_STRUCTURES_H_


typedef enum
{ 
  NA_NULL = 0,
  NA_LOOPBACK,
  NA_BROADCAST,
  NA_IP,
} netadrtype_t;

typedef struct netadr_s {
  netadrtype_t	type;
  unsigned int	ip;
  unsigned short	port;
} netadr_t;

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
};

// Copied from hl2sdk so we don't have to include all that junk
typedef struct netpacket_s 
{
  netadr_t		from;		// sender IP
  int				source;		// received source 
  double			received;	// received time
  unsigned char	*data;		// pointer to raw packet data
  struct fake_bf_read			message;	// easy bitbuf data access
  int				size;		// size in bytes
  int				wiresize;   // size in bytes before decompression
  bool			stream;		// was send as stream
  struct netpacket_s *pNext;	// for internal use, should be NULL in public
} netpacket_t;

#endif // _CUSTOM_FAKELAG_NET_STRUCTURES_H_
