//
//  MQTTKit.h
//  MQTTKit
//
//  Created by Jeff Mesnil on 22/10/2013.
//  Copyright (c) 2013 Jeff Mesnil. All rights reserved.
//  Copyright 2012 Nicholas Humfrey. All rights reserved.
//

typedef enum MQTTConnectionReturnCode : NSUInteger {
    ConnectionAccepted,
    ConnectionRefusedUnacceptableProtocolVersion,
    ConnectionRefusedIdentiferRejected,
    ConnectionRefusedServerUnavailable,
    ConnectionRefusedBadUserNameOrPassword,
    ConnectionRefusedNotAuthorized
} MQTTConnectionReturnCode;

typedef enum MQTTQualityOfService : NSUInteger {
    AtMostOnce,
    AtLeastOnce,
    ExactlyOnce
} MQTTQualityOfService;

#ifdef WITH_TLS

extern NSString *const MQTTKitTLSVersion1;
extern NSString *const MQTTKitTLSVersion1_1;
extern NSString *const MQTTKitTLSVersion1_2;

#endif

#pragma mark - MQTT Message

@interface MQTTMessage : NSObject

@property (readonly, assign) unsigned short mid;
@property (readonly, copy) NSString *topic;
@property (readonly, copy) NSData *payload;
@property (readonly, assign) BOOL retained;

- (NSString *)payloadString;

@end

typedef void (^MQTTSubscriptionCompletionHandler)(NSArray *grantedQos);
typedef void (^MQTTMessageHandler)(MQTTMessage *message);
typedef void (^MQTTDisconnectionHandler)(NSUInteger code);

#pragma mark - MQTT Client

@class MQTTClient;

@interface MQTTClient : NSObject {
    struct mosquitto *mosq;
}

@property (readwrite, copy) NSString *clientID;
@property (readwrite, copy) NSString *host;
@property (readwrite, assign) unsigned short port;
@property (readwrite, copy) NSString *username;
@property (readwrite, copy) NSString *password;
@property (readwrite, assign) unsigned short keepAlive;
@property (readwrite, assign) unsigned int reconnectDelay; // in seconds (default is 1)
@property (readwrite, assign) unsigned int reconnectDelayMax; // in seconds (default is 1)
@property (readwrite, assign) BOOL reconnectExponentialBackoff; // wheter to backoff exponentially the reconnect attempts (default is NO)
@property (readwrite, assign) BOOL cleanSession;
@property (readonly, assign) BOOL connected;
@property (nonatomic, copy) MQTTMessageHandler messageHandler;
@property (nonatomic, copy) MQTTDisconnectionHandler disconnectionHandler;
#ifdef WITH_TLS
/* Path for ca file which verify the server certificate */
@property (nonatomic, copy) NSString *tlsCafile;
/* Path for client certificate. If tlsCerKeyPath is NULL it will not work */
@property (nonatomic, copy) NSString *tlsCerPath;
/* Path for client certificate key. If tlsCerPath is NULL it will not work */
@property (nonatomic, copy) NSString *tlsCerKeyPath;
/* Default is tlsv1.2 */
@property (nonatomic, copy) NSString *tlsVersion;
/* List for ciphers that client support */
@property (nonatomic, copy) NSString *tlsCiphers;
/* If YES, client will verify server certificate */
@property (nonatomic, assign) BOOL tlsPeerCertVerify;
/* Preverify should have already checked expiry, revocation.
 * If tlsInsecure is false we need to verify the hostname otherwise return preverify result directly*/
@property (nonatomic, assign) BOOL tlsInsecure;

/* If you want to use pre-share key this part function, SET MACRO REAL_WITH_TLS_PSK = 1 */
/*
 If tlsPsk is not NULL, client will set PSK client callback
 default is NULL
 
 Document:
 A client application must provide a callback function which is called when the client is sending the ClientKeyExchange message to the server.
 
 The purpose of the callback function is to select the PSK identity and the pre-shared key to use during the connection setup phase.
 */
@property (nonatomic, copy) NSString *tlsPsk;
@property (nonatomic, copy) NSString *tlsPskIdentity;
#endif

+ (void) initialize;
+ (NSString*) version;

- (MQTTClient*) initWithClientId: (NSString *)clientId;
- (MQTTClient*) initWithClientId: (NSString *)clientId
                    cleanSession: (BOOL )cleanSession;

- (int) setMaxInflightMessages:(NSUInteger)maxInflightMessages;
- (void) setMessageRetry: (NSUInteger)seconds;

#pragma mark - Connection

- (int) connectWithCompletionHandler:(void (^)(MQTTConnectionReturnCode code))completionHandler;
- (int) connectToHost: (NSString*)host
     completionHandler:(void (^)(MQTTConnectionReturnCode code))completionHandler;
- (int) disconnectWithCompletionHandler:(MQTTDisconnectionHandler)completionHandler;
- (int) reconnect;
- (int)setWillData:(NSData *)payload
            toTopic:(NSString *)willTopic
            withQos:(MQTTQualityOfService)willQos
             retain:(BOOL)retain;
- (int)setWill:(NSString *)payload
        toTopic:(NSString *)willTopic
        withQos:(MQTTQualityOfService)willQos
         retain:(BOOL)retain;
- (int)clearWill;

#pragma mark - Publish

- (int)publishData:(NSData *)payload
            toTopic:(NSString *)topic
            withQos:(MQTTQualityOfService)qos
             retain:(BOOL)retain
  completionHandler:(void (^)(int mid))completionHandler;
- (int)publishString:(NSString *)payload
              toTopic:(NSString *)topic
              withQos:(MQTTQualityOfService)qos
               retain:(BOOL)retain
    completionHandler:(void (^)(int mid))completionHandler;

#pragma mark - Subscribe

- (int)subscribe:(NSString *)topic
withCompletionHandler:(MQTTSubscriptionCompletionHandler)completionHandler;
- (int)subscribe:(NSString *)topic
          withQos:(MQTTQualityOfService)qos
completionHandler:(MQTTSubscriptionCompletionHandler)completionHandler;
- (int)unsubscribe: (NSString *)topic
withCompletionHandler:(void (^)(void))completionHandler;

@end
