/*
  The oSIP library implements the Session Initiation Protocol (SIP -rfc3261-)
  Copyright (C) 2001,2002,2003  Aymeric MOIZARD jack@atosc.org
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _SDP_NEGOC_H_
#define _SDP_NEGOC_H_
//#include "sip_def.h"
#include "sdp_message.h"


/**
 * @file sdp_negoc.h
 * @brief oSIP and SDP offer/answer model Routines
 *
 * The SDP offer/answer model is where most SIP interoperability issue
 * comes from.  The SDP specification (rfc2327.txt) is often not fully
 * respected. As an example, most SIP applications forget to add the
 * mandatory 's' field in the SDP packet. Another mistake is to assume
 * that an SDP packet don't need a 'p' and a 'e' field. Even if they
 * are both optional, at least of those is mandatory! I have never
 * seen ONE implementation that send at least one 'p' or 'e' field!!
 * <BR>For all the reasons, that make negotiation a hard task, I have
 * decided to provide a helpful facility to build SDP answer from an
 * SDP offer. (This facility does not help to build the compliant offer)
 * Of course, after the SDP negotiator has been executed and produced
 * a valid response, you can still modify your SDP answer to add
 * attributes or modify anything. You always keep the entire control
 * over it.
 * <H2>Do you need the negotiator</H2>
 * If you are planning a simple application, I advise you to use it.
 * Advanced applications may find it inappropriate, but as you can
 * modify the SDP answer after running the negotiation, I see no reason
 * why you should not use it. The only goal of the SDP negotiator is
 * to make sure only one line of audio codec is accepted (the first one)
 * and only one line of video codec is accepted (the first one). It
 * also remove from the media lines, the codec that you don't support
 * without asking you. (Also, you can still refuse the codec you support.)
 * <BR>Using the negotiator, your only task is to check/add/remove
 * the media attributes. 
 * 
 * <H2>How-To</H2>
 * Using the SDP negotiator is simple. An example is provided in the
 * test directory as 'torture_sdp.c'. It parses a SDP packet from
 * a file (a sample is available in conf/) and produce the answer
 * that would be made with a basic configuration where 4 audio codecs
 * are supported.
 * <BR>When starting your application, you simply configure the global
 * sdp_config_t element: you'll set you username, ip address and some
 * general informations about you that every SDP packet must contain.
 * As a second action, you will register all the codec you support.
 * (audio, video and 'other' codecs).
 * <BR>After that, you will also register a set of method used to
 * accept the codec. The return code of those method will accept or
 * refused the supported codec for this specific session.
 * <pre><code>
 *  sdp_config_set_fcn_accept_audio_codec(&application_accept_audio_codec);
 *  sdp_config_set_fcn_accept_video_codec(&application_accept_video_codec);
 *  sdp_config_set_fcn_accept_other_codec(&application_accept_other_codec);
 *  sdp_config_set_fcn_get_audio_port(&application_get_audio_port);
 * </pre></code>
 * <BR>When you need to create an answer, the following code will create
 * the SDP packet:
 * <pre><code>
 *  sdp_context_t *context;
 *  
 *  sdp_message_t *dest;
 *  i = sdp_context_init(&context);
 *  i = sdp_context_set_mycontext(context, (void *)ua_context);
 *  i = sdp_context_set_remote_sdp(context, sdp);
 *  if (i!=0) {
 *    fprintf(stdout, "Initialisation of context failed. Could not negociate");
 *  } else {
 *    fprintf(stdout, "Trying to execute a SIP negociation:");
 *    i = sdp_context_execute_negociation(context);
 *    fprintf(stdout, "return code: %i",i);
 *    if (i==200)
 *     {
 *       dest = sdp_context_get_local_sdp(context);
 *       fprintf(stdout, "SDP answer:");
 *       i = sdp_2char(dest, &result);
 *       if (i!=0)
 *         fprintf(stdout, "Error found in SDP answer while printing");
 *       else
 *         fprintf(stdout, "%s", result);
 *       sfree(result);
 *     }
 *    sdp_context_free(context);
 *    sfree(context);
 *    return 0;
 *  }
 * </pre></code>
 * <BR>Notice the presence of sdp_context_set_mycontext() which can add
 * a store the address of your own context (probably related to your call).
 * This is very useful if you need to know inside the callback which call
 * this negotiation belongs to.
 */

/**
 * @defgroup oSIP_OAM oSIP and SDP offer/answer model Handling
 * @ingroup oSIP
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct sdp_context_t sdp_context_t;

struct sdp_context_t
{
	void *mycontext;		/* add the pointer to your personal context */
	sdp_message_t *remote;
	sdp_message_t *local;
};

/**
 * Allocate a bandwidth element.
 * @param ctx The element to work on.
 */
  int sdp_context_init (sdp_context_t ** ctx);
/**
 * Free a bandwidth element.
 * @param ctx The element to work on.
 */
  void sdp_context_free (sdp_context_t * ctx);

/**
 * Set the context associated to this negotiation.
 * @param ctx The element to work on.
 * @param value A pointer to your personal context.
 */
  int sdp_context_set_mycontext (sdp_context_t * ctx, void *value);
/**
 * Get the context associated to this negotiation.
 * @param ctx The element to work on.
 */
  void *sdp_context_get_mycontext (sdp_context_t * ctx);

/**
 * Set the local SDP packet associated to this negotiation.
 * NOTE: This is done by the 'negotiator'. (You only need to give
 * the remote SDP packet)
 * @param ctx The element to work on.
 * @param sdp The local SDP packet.
 */
  int sdp_context_set_local_sdp (sdp_context_t * ctx, sdp_message_t * sdp);
/**
 * Get the local SDP packet associated to this negotiation.
 * @param ctx The element to work on.
 */
  sdp_message_t *sdp_context_get_local_sdp (sdp_context_t * ctx);
/**
 * Set the remote SDP packet associated to this negotiation.
 * @param ctx The element to work on.
 * @param sdp The remote SDP packet.
 */
  int sdp_context_set_remote_sdp (sdp_context_t * ctx, sdp_message_t * sdp);
/**
 * Get the remote SDP packet associated to this negotiation.
 * @param ctx The element to work on.
 */
  sdp_message_t *sdp_context_get_remote_sdp (sdp_context_t * ctx);

/**
 * Structure for payload management. Each payload element
 * represents one codec of a media line.
 * @defvar payload_t
 */
  typedef struct payload_t payload_t;

  struct payload_t
  {
    char *payload;
    /*  char *port; this must be assigned by the application dynamicly */
    char *number_of_port;
    char *proto;
    char *c_nettype;
    char *c_addrtype;
    char *c_addr;
    char *c_addr_multicast_ttl;
    char *c_addr_multicast_int;
    /* rtpmap (rcvonly and other attributes are added dynamicly) */
    char *a_rtpmap;
  };


/**
 * Allocate a payload element.
 * @param payload The payload.
 */
  int payload_init (payload_t ** payload);
/**
 * Free a payload element.
 * @param payload The payload.
 */
  void payload_free (payload_t * payload);

/**
 * Structure for storing the global configuration management.
 * The information you store here is used when computing a
 * remote SDP packet to build a compliant answer.
 * The main objectives is to:
 *    * automaticly refuse unknown media.
 *    * accept some of the known media.
 *    * make sure the SDP answer match the SDP offer.
 *    * simplify the SDP offer/answer model, as all unknown media
 *      are refused without any indication to the application layer.
 *    * In any case, you can still modify the entire SDP packet after
 *      a negotiation if you are not satisfied by the negotiation result.
 * @defvar sdp_config_t
 */
  typedef struct sdp_config_t sdp_config_t;

  struct sdp_config_t
  {
    char *o_username;
    char *o_session_id;
    char *o_session_version;
    char *o_nettype;
    char *o_addrtype;
    char *o_addr;

    char *c_nettype;
    char *c_addrtype;
	char *c_addr;

	char *c_nettype6;
	char *c_addrtype6;
	char *c_addr6;

    char *c_addr_multicast_ttl;
    char *c_addr_multicast_int;

    osip_list_t audio_codec;
    osip_list_t video_codec;
    osip_list_t other_codec;

	int  ptime;

    int (*fcn_set_info) (void *, sdp_message_t *);
    int (*fcn_set_uri) (void *, sdp_message_t *);

    int (*fcn_set_emails) (void *, sdp_message_t *);
    int (*fcn_set_phones) (void *, sdp_message_t *);
    int (*fcn_set_attributes) (void *, sdp_message_t *, int);
    int (*fcn_accept_audio_codec) (void *, char *, char *, int, char *);
    int (*fcn_accept_video_codec) (void *, char *, char *, int, char *);
    int (*fcn_accept_other_codec) (void *, char *, char *, char *, char *);
    char *(*fcn_get_audio_port) (void *, int);
    char *(*fcn_get_video_port) (void *, int);
    char *(*fcn_get_other_port) (void *, int);
	char *(*fcn_get_c_addr) (int);

  };

//sdp_config_t *sdp_config_get();

/**
 * Initialise (and Allocate) a sdp_config element (this element is global).
 * This method must be called when the application is started.
 */
  int sdp_config_init (sdp_config_t **cfg);
/**
 * Free resource stored by a sdp_config element.
 * This method must be called once when the application is stopped.
 */
  void sdp_config_free (sdp_config_t *cfg);

/**
 * Set the local username ('o' field) of all local SDP packet.
 * @param tmp The username.
 */
  int sdp_config_set_o_username (sdp_config_t * config,char *tmp);
/**
 * Set the local session id ('o' field) of all local SDP packet.
 * WARNING: this field should be updated for each new SDP packet?
 * @param tmp The session id.
 */
  int sdp_config_set_o_session_id (sdp_config_t * config,char *tmp);
/**
 * Set the local session version ('o' field) of all local SDP packet.
 * WARNING: this field should be updated for each new SDP packet?
 * @param tmp The session version.
 */
  int sdp_config_set_o_session_version (sdp_config_t * config,char *tmp);
/**
 * Set the local network type ('o' field) of all local SDP packet.
 * @param tmp The network type.
 */
  int sdp_config_set_o_nettype (sdp_config_t * config,char *tmp);
/**
 * Set the local address type ('o' field) of all local SDP packet.
 * @param tmp The address type.
 */
  int sdp_config_set_o_addrtype (sdp_config_t * config,char *tmp);
/**
 * Set the local IP address ('o' field) of all local SDP packet.
 * @param tmp The IP address.
 */
  int sdp_config_set_o_addr (sdp_config_t * config,char *tmp);

/**
 * Set the local network type ('c' field) of all local SDP packet.
 * @param tmp The network type.
 */
  int sdp_config_set_c_nettype (sdp_config_t * config,char *tmp);
/**
 * Set the local address type ('c' field) of all local SDP packet.
 * @param tmp The address type.
 */
  int sdp_config_set_c_addrtype (sdp_config_t * config,char *tmp);
/**
 * Set the local IP address ('c' field) of all local SDP packet.
 * @param tmp The IP address.
 */
  int sdp_config_set_c_addr (sdp_config_t * config,char *tmp);

  int sdp_config_set_c_nettype6 (sdp_config_t * config,char *tmp);
  int sdp_config_set_c_addrtype6 (sdp_config_t * config,char *tmp);
  int sdp_config_set_c_addr6 (sdp_config_t * config,char *tmp);


/**
 * Set the local ttl for multicast address ('c' field) of all local SDP packet.
 * @param tmp The ttl for multicast address.
 */
  int sdp_config_set_c_addr_multicast_ttl (sdp_config_t * config,char *tmp);
/**
 * Set the local int for multicast address ('c' field) of all local SDP packet.
 * @param tmp The int for multicast address.
 */
  int sdp_config_set_c_addr_multicast_int (sdp_config_t * config,char *tmp);

/**
 * Add a supported audio codec.
 * Those codecs will be accepted as long as you return 0 when
 * the callback 'fcn_accept_audio_codec' is called with the specific payload.
 * @param payload The payload.
 * @param number_of_port The number of port (channel) for this codec.
 * @param proto The protocol.
 * @param c_nettype The network type in the 'c' field.
 * @param c_addrtype The address type in the 'c' field.
 * @param c_addr The address in the 'c' field.
 * @param c_addr_multicast_ttl The ttl for multicast address in the 'c' field.
 * @param c_addr_multicast_int The int for multicast address in the 'c' field.
 * @param a_rtpmap The rtpmap attribute in the 'a' field.
 */
  int sdp_config_add_support_for_audio_codec (sdp_config_t * config,char *payload,
					      char *number_of_port,
					      char *proto, char *c_nettype,
					      char *c_addrtype, char *c_addr,
					      char *c_addr_multicast_ttl,
					      char *c_addr_multicast_int,
					      char *a_rtpmap);
/**
 * Add a supported video codec.
 * Those codecs will be accepted as long as you return 0 when
 * the callback 'fcn_accept_video_codec' is called with the specific payload.
 * @param payload The payload.
 * @param number_of_port The number of port (channel) for this codec.
 * @param proto The protocol.
 * @param c_nettype The network type in the 'c' field.
 * @param c_addrtype The address type in the 'c' field.
 * @param c_addr The address in the 'c' field.
 * @param c_addr_multicast_ttl The ttl for multicast address in the 'c' field.
 * @param c_addr_multicast_int The int for multicast address in the 'c' field.
 * @param a_rtpmap The rtpmap attribute in the 'a' field.
 */
  int sdp_config_add_support_for_video_codec (sdp_config_t * config,char *payload,
					      char *number_of_port,
					      char *proto, char *c_nettype,
					      char *c_addrtype, char *c_addr,
					      char *c_addr_multicast_ttl,
					      char *c_addr_multicast_int,
					      char *a_rtpmap);
/**
 * Add a supported (non-audio and non-video) codec.
 * Those codecs will be accepted as long as you return 0 when
 * the callback 'fcn_accept_other_codec' is called with the specific payload.
 * @param payload The payload.
 * @param number_of_port The number of port (channel) for this codec.
 * @param proto The protocol.
 * @param c_nettype The network type in the 'c' field.
 * @param c_addrtype The address type in the 'c' field.
 * @param c_addr The address in the 'c' field.
 * @param c_addr_multicast_ttl The ttl for multicast address in the 'c' field.
 * @param c_addr_multicast_int The int for multicast address in the 'c' field.
 * @param a_rtpmap The rtpmap attribute in the 'a' field.
 */
  int sdp_config_add_support_for_other_codec (sdp_config_t * config,char *payload,
					      char *number_of_port,
					      char *proto, char *c_nettype,
					      char *c_addrtype, char *c_addr,
					      char *c_addr_multicast_ttl,
					      char *c_addr_multicast_int,
					      char *a_rtpmap);

#ifndef DOXYGEN
/**
 * Free resource in the global sdp_config..
 */
  int sdp_config_remove_audio_payloads (sdp_config_t * config);
/**
 * Free resource in the global sdp_config..
 */
  int sdp_config_remove_video_payloads (sdp_config_t * config);
/**
 * Free resource in the global sdp_config..
 */
  int sdp_config_remove_other_payloads (sdp_config_t * config);
#endif

/**
 * Set the callback for setting info ('i' field) in a local SDP packet.
 * This callback is called once each time we need an 'i' field.
 * @param fcn The callback.
 */
  int sdp_config_set_fcn_set_info (sdp_config_t * config,int (*fcn) (sdp_context_t *, sdp_message_t *));
/**
 * Set the callback for setting a URI ('u' field) in a local SDP packet.
 * This callback is called once each time we need an 'u' field.
 * @param fcn The callback.
 */
  int sdp_config_set_fcn_set_uri (sdp_config_t * config,int (*fcn) (sdp_context_t *, sdp_message_t *));
/**
 * Set the callback for setting an email ('e' field) in a local SDP packet.
 * This callback is called once each time we need an 'e' field.
 * @param fcn The callback.
 */
  int sdp_config_set_fcn_set_emails (sdp_config_t * config,int (*fcn) (sdp_context_t *, sdp_message_t *));
/**
 * Set the callback for setting a phone ('p' field) in a local SDP packet.
 * This callback is called once each time we need an 'p' field.
 * @param fcn The callback.
 */
  int sdp_config_set_fcn_set_phones (sdp_config_t * config,int (*fcn) (sdp_context_t *, sdp_message_t *));
/**
 * Set the callback for setting an attribute ('a' field) in a local SDP packet.
 * This callback is called once each time we need an 'a' field.
 * @param fcn The callback.
 */
  int
    sdp_config_set_fcn_set_attributes (sdp_config_t * config,int (*fcn)
				       (sdp_context_t *, sdp_message_t *, int));
/**
 * Set the callback used to accept a codec during a negotiation.
 * This callback is called once each time we need to accept a codec.
 * @param fcn The callback.
 */
  int
    sdp_config_set_fcn_accept_audio_codec (sdp_config_t * config,int (*fcn)
					   (sdp_context_t *, char *, char *,
					    int, char *));

  void sdp_config_set_ptime(sdp_config_t * config,int ptime);
  
/**
 * Set the callback used to accept a codec during a negotiation.
 * This callback is called once each time we need to accept a codec.
 * @param fcn The callback.
 */
  int
    sdp_config_set_fcn_accept_video_codec (sdp_config_t * config,int (*fcn)
					   (sdp_context_t *, char *, char *,
					    int, char *));
/**
 * Set the callback used to accept a codec during a negotiation.
 * This callback is called once each time we need to accept a codec.
 * @param fcn The callback.
 */
  int
    sdp_config_set_fcn_accept_other_codec (sdp_config_t * config,int (*fcn)
					   (sdp_context_t *, char *, char *,
					    char *, char *));
/**
 * Set the callback for setting the port number ('m' field) in a local SDP packet.
 * This callback is called once each time a 'm' line is accepted.
 * @param fcn The callback.
 */
  int sdp_config_set_fcn_get_audio_port (sdp_config_t * config,char *(*fcn) (sdp_context_t *, int));
/**
 * Set the callback for setting the port number ('m' field) in a local SDP packet.
 * This callback is called once each time a 'm' line is accepted.
 * @param fcn The callback.
 */
  int sdp_config_set_fcn_get_video_port (sdp_config_t * config,char *(*fcn) (sdp_context_t *, int));
/**
 * Set the callback for setting the port number ('m' field) in a local SDP packet.
 * This callback is called once each time a 'm' line is accepted.
 * @param fcn The callback.
 */
  int sdp_config_set_fcn_get_other_port (sdp_config_t * config,char *(*fcn) (sdp_context_t *, int));

  int sdp_config_set_fcn_get_c_addr(sdp_config_t * config, char *(*fcn)(int));

/**
 * Start the automatic negotiation for a UA
 * NOTE: You can previously set context->mycontext to point to your
 * personal context. This way you'll get access to your personal context
 * in the callback and you can easily take the correct decisions.
 * After this method is called, the negotiation will happen and
 * callbacks will be called. You can modify, add, remove SDP fields,
 * and accept and refuse the codec from your preferred list by using
 * those callbacks.
 * Of course, after the negotiation happen, you can modify the
 * SDP packet if you wish to improve it or just refine some attributes.
 * @param ctx The context holding the remote SDP offer.
 */
  int sdp_context_execute_negotiation (sdp_config_t * config,sdp_context_t * ctx,int aPort,int vPort,const char *c_addr);

  int sdp_confirm_ptime(sdp_config_t * config,sdp_context_t *context, sdp_message_t *remote, sdp_message_t ** dest);

  int sdp_build_offer (sdp_config_t * config, sdp_message_t ** sdp, char *audio_port,
		       char *video_port,char *c_addr);
  int __sdp_build_offer (sdp_config_t * config,sdp_context_t * con, sdp_message_t ** sdp, char *audio_port,
			 char *video_port, char *audio_codec, char *video_codec);

  /* for non "on-hold sdp" in outgoing invite */
  int sdp_message_put_on_hold (sdp_message_t * sdp);
  /* for an "on hold sdp" in outgoing invite */
  int sdp_message_put_off_hold (sdp_message_t * sdp);


#ifndef DOXYGEN
/* deprecated for spelling reasons... */
#define sdp_context_execute_negociation sdp_context_execute_negotiation
#endif

/** @} */


#ifdef __cplusplus
}
#endif

#endif /*_SDP_NEGOC_H_ */
