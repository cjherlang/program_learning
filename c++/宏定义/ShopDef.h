#pragma once

#include "enumhelper.h"

#ifndef _SERVER_
#include "TypeDef.h"
#include "Buffer.h"
#include "NetPacket.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <math.h>
#else
#include "platform.h"
#endif



enum PaymentServicePlatform {
    InvalidPlatform = -1,
    MyGame = 0,	// 游戏自身
    Diandian,
    NdStore,	// 91移动开发平台
    AppStore,
    TbStore,	// 同步推4
    GooglePlay,	//15
    MMBilling,
    CTEStore,
    KYStore,
    WpayStore,
    UniconStore,//10 沃支付
    PPStore,
    MMSMSBilling,
    QIHUBilling,
    G3Billing,
    LenovoStore,//15
    UniPay,		//联通沃商店
	BaiduStore,
	XiaomiPay,
	HuaweiPay,
	OppoPay,    //20  
    DJPay,      //点金支付
    WNPay,       //蜗牛支付

    /*
    AliPay\TenPay\DefPay is pay method in diandian etc.
    new payment just put before it
    */
    AliPay = 100,
    TenPay,
    PhCardPay,      //phone card
    GCardPay,       //game card 
    DefPay,         //default payment, may contains all pay method
};

enum PaymentMethodType {
    PAYMENT_METHOD_SMS = 1,
    PAYMENT_METHOD_BANK = 1<<1,
    PAYMENT_METHOD_PREPAID_CARD = 1<<2,
    PAYMENT_METHOD_ALL = 0xFFFFFFFF,
};

enum PaymentCurrencyType {
    PAYMENT_CURRENCY_MONEY = 1,
    PAYMENT_CURRENCY_GOLDS = 2,
    PAYMENT_CURRENCY_COUPON = 3,
};

enum ShopPayPointType {
    SHOP_PAY_POINT_SHOP = 1,				// 充值
    SHOP_PAY_POINT_CHANGE_CARD = 1 << 1,	// 换牌
    SHOP_PAY_POINT_PROPERTY_SHOP = 1 << 2,  // 道具商城
    SHOP_PAY_POINT_CONTINOUS_MATCH = 1 << 3,	// 连续赛
    SHOP_PAY_POINT_SUPER_SHOP = 1 << 4,     //小额支付
};

enum PropertyType {
    PROPERTY_DEFAULT = 0,
    PROPERTY_GOLD_COINS = 1,	// 金币
    PROPERTY_GOLD_NOTES,		// 金券
    PROPERTY_DIAMONDS,			// 钻石
    PROPERTY_HORN,				// 小喇叭
    PROPERTY_TOTAL_CHARGE_FEE,	// 充值金额
    PROPERTY_TICKET,			// 抽奖券
    PROPERTY_COUPON,			// 兑换券
    PROPERTY_MATCH_NOTES,		// 参赛券
    LITTLE_PAY,                 //小额支付
    PROPERTY_KICK_CARD, 		// 踢人卡
    PROPERTY_ALL = 0xFFFFFFFF, 
};

static const char* g_strPropretyTypeName[][2] = {
    {"",			""},
    {"金币",		""},
    {"金券",		"张"},
    {"钻石",		"颗"},
    {"小喇叭",		"个"},
    {"充值金额",	"元"},
    {"抽奖券",		"张"},
    {"兑换券",		"张"},
    {"参赛券",		"张"},
    {"超值礼包",    "个"},
    {"踢人卡",		"张"},
};

DECL_ENUM_HELPER(PaymentServicePlatform);

DECL_ENUM_HELPER(PropertyType);

DECL_ENUM_HELPER(PaymentCurrencyType);

typedef int32 stPropertyId;
typedef int32 PropertyVersion;

struct stProperty {
public:
    stPropertyId GetPropertyId() const { return m_propertyId; }
    void SetPropertyId(const stPropertyId &val) { m_propertyId = val; }
    std::string GetName() const { return m_name; }
    void SetName(const std::string &val) { m_name = val; }

    const std::map<stPropertyId, int32>* GetPropertyEffectAttributes() const { return &m_effectAttributes; }

    static stPropertyId GetPropertyId(const std::string &p);

    int32 & operator[](const stPropertyId attr) {
        return m_effectAttributes[attr];
    }

    virtual void ToPacket( INetPacket& packet ) 
    {		
        packet << m_propertyId << m_name;

        int32 countOfEffectAttributes = m_effectAttributes.size();
        packet << countOfEffectAttributes;
        for (std::map<stPropertyId, int32>::const_iterator it = m_effectAttributes.begin(); it != m_effectAttributes.end(); ++it) {
            packet << (int32)it->first << it->second;
        }
    }

    virtual void PacketTo( INetPacket& packet ) 
    {
        packet >> m_propertyId >> m_name;

        int32 countOfEffectAttributes = 0;
        packet >> countOfEffectAttributes;
        for (int i=0; i<countOfEffectAttributes; i++) {
            int32 attr = 0;
            int32 val = 0;
            packet >> attr >> val;
            m_effectAttributes[(stPropertyId)attr] = val;
        }
    }

#ifndef _SERVER_
    virtual void ToStream( FileBuffer& packet ) 
    {
        packet << m_propertyId << m_name;

        int32 countOfEffectAttributes = m_effectAttributes.size();
        packet << countOfEffectAttributes;
        for (std::map<stPropertyId, int32>::const_iterator it = m_effectAttributes.begin(); it != m_effectAttributes.end(); ++it) {
            packet << (int32)it->first << it->second;
        }
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        packet >> m_propertyId >> m_name;

        int32 countOfEffectAttributes = 0;
        packet >> countOfEffectAttributes;
        for (int i=0; i<countOfEffectAttributes; i++) {
            int32 attr = 0;
            int32 val = 0;
            packet >> attr >> val;
            m_effectAttributes[(stPropertyId)attr] = val;
        }
    }
#endif

protected:
    stPropertyId	m_propertyId;
    std::string m_name;
    std::map<stPropertyId, int32> m_effectAttributes;
};

struct stShopItemBasicInfo {
public:
    stShopItemBasicInfo(const std::string& platform);

    stShopItemBasicInfo(PaymentServicePlatform platform);

    std::string GetPlatform() const { return m_platform; }
    std::string GetShopId() const { return m_shopId; }
    void SetShopId(std::string val) { m_shopId = val; }
    stPropertyId GetPropertyId() const { return m_propertyId; }
    void SetPropertyId(stPropertyId val) { m_propertyId = val; }
    std::string GetItemId() const { return m_itemId; }
    void SetItemId(std::string val) { m_itemId = val; }
    std::string GetGameItemId() const { return m_gameItemId; }
    void SetGameItemId(std::string val) { m_gameItemId = val; }
    uint32 GetAcceptQuantity() const { return m_acceptQuantity; }
    void SetAcceptQuantity(int32 val) { m_acceptQuantity = (uint32)val; }

#ifdef _SERVER_
    std::string GetSupportOS() const { return m_supportOS; }
    void SetSupportOS(std::string val) { m_supportOS = "|" + val + "|"; m_isSupportAllOS = val == "*"; }

    std::string GetSupportChannel() const { return m_supportChannel; }
    void SetSupportChannel(std::string val) { m_supportChannel = "|" + val + "|"; m_isSupportAllChannel = val == "*"; }

    bool IsSupport(const std::string& clientOS, const std::string& clientChannel) const {
        bool isNotSupportChannel = m_supportChannel.find("|-" + clientChannel + "|") != std::string::npos;
        bool isNotSupportOS = m_supportOS.find("|-" + clientOS + "|") != std::string::npos;

        if (isNotSupportOS || isNotSupportChannel) {
            return false;
        }

        //support rule : if exist -xx(not support xx) ,then it will support xxx. ('-xx|xxx' can't be wirte in support rules)
        bool isSupportChannel = m_isSupportAllChannel || m_supportChannel.find("-") != std::string::npos;
        isSupportChannel = isSupportChannel || m_supportChannel.find("|" + clientChannel + "|") != std::string::npos;
        bool isSupportOS = m_isSupportAllOS || m_supportOS.find("-") != std::string::npos;
        isSupportOS = isSupportOS || m_supportOS.find("|" + clientOS + "|") != std::string::npos;

        return isSupportOS && isSupportChannel;
    };
#endif

    int32 GetPaymentMethod() const;
    bool ContainsPaymentMethod( int paymentMethods ) const;
    bool AcceptsQuantity() const;

    virtual void ToPacket( INetPacket& packet ) 
    {
        packet << m_platform << m_shopId << m_propertyId << m_itemId << m_gameItemId << m_paymentMethod << m_acceptQuantity;
    }

    virtual void PacketTo( INetPacket& packet ) 
    {
        packet >> m_platform >> m_shopId >> m_propertyId >> m_itemId >> m_gameItemId >> m_paymentMethod >> m_acceptQuantity;
    }

#ifndef _SERVER_
    virtual void ToStream( FileBuffer& packet ) 
    {		
        packet << m_platform << m_shopId << m_propertyId << m_itemId << m_gameItemId << m_paymentMethod << m_acceptQuantity;
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        packet >> m_platform >> m_shopId >> m_propertyId >> m_itemId >> m_gameItemId >> m_paymentMethod >> m_acceptQuantity;
    }
#endif

protected:
    std::string		m_platform;
    std::string		m_shopId;
    stPropertyId	m_propertyId;
    std::string		m_itemId;
    std::string		m_gameItemId;
    int32			m_paymentMethod;
    uint32			m_acceptQuantity;

#ifdef _SERVER_
    std::string		m_supportOS;
    std::string		m_supportChannel;
    bool			m_isSupportAllOS;
    bool			m_isSupportAllChannel;
#endif

    stShopItemBasicInfo() : m_paymentMethod(0), m_acceptQuantity(1)
#ifdef _SERVER_
        , m_isSupportAllChannel(true)
        , m_isSupportAllOS(true)
#endif
    {
    }

};

struct stShopItemDetailInfo 
    : public stShopItemBasicInfo {
public:
    stShopItemDetailInfo(PaymentServicePlatform platform)
        : stShopItemBasicInfo(platform)
        , m_itemPriceValue(0.0f)
        , m_itemPayPoint(0)
        , m_itemPayCurrency(0)
        , m_propertyQuantity(0)
        , m_propertyGiftQuantity(0)
    {
    }

    stShopItemDetailInfo(const std::string& platform)
        : stShopItemBasicInfo(platform)
        , m_itemPriceValue(0.0f)
        , m_itemPayPoint(0)
        , m_itemPayCurrency(0)
        , m_propertyQuantity(0)
        , m_propertyGiftQuantity(0)
    {
    }

    stShopItemDetailInfo()
        : m_itemPriceValue(0.0f)
        , m_itemPayPoint(0)
        , m_itemPayCurrency(0)
        , m_propertyQuantity(0)
        , m_propertyGiftQuantity(0)
    {
    }

    std::string GetItemName() const { return m_itemName; }
    void SetItemName(std::string &val) { m_itemName = val; }
    std::string GetItemType() const { return m_itemType; }
    void SetItemType(std::string &val) { m_itemType = val; }
    float GetItemPriceValue() const { return m_itemPriceValue; }
    void SetItemPriceValue(float val) { m_itemPriceValue = val; }
    std::string GetItemPriceType() const { return m_itemPriceUnit; }
    void SetItemPriceUnit(std::string &val) { m_itemPriceUnit = val; }
    int32 GetItemPayCurrency() const { return m_itemPayCurrency; }
    void SetItemPayCurrency(int32 val) { m_itemPayCurrency = val; }
    void SetItemPayCurrency(std::string &val);
    int32 GetItemPayPoint() const { return m_itemPayPoint; }
    void SetItemPayPoint(int32 val) { m_itemPayPoint = val; }
    void SetItemPayPoint(std::string &val);
    int32 GetPropertyQuantity() const { return m_propertyQuantity; }
    void SetPropertyQuantity(int32 val) { m_propertyQuantity = val; }
    int32 GetPropertyGiftQuantity() const { return m_propertyGiftQuantity; }
    void SetPropertyGiftQuantity(int32 val) { m_propertyGiftQuantity = val; }
    int32 GetPropertyTotalQuantity() const { return m_propertyQuantity + m_propertyGiftQuantity; }
    void SetEffectAttributes(int32 key,int32 val) { m_effectAttributes[key] = val; }


    bool ContainsEntryPoint (int entryPoints) const;

    std::map<stPropertyId, int32> &GetFinalProperyEffectAttributes() {
        return m_finalEffectAttributes;
    }

    int32 GetProperyEffectAttribute(stPropertyId attr, int32 defaultValue = 0) const;
    int32 GetFinalProperyEffectAttribute(stPropertyId attr, int32 defaultValue = 0) const;

    void MergePropertyEffectAttributes (const stProperty& property) {
        assert(property.GetPropertyId() == this->GetPropertyId());
        
        m_finalEffectAttributes = m_effectAttributes;

        for (std::map<stPropertyId, int32>::const_iterator it = property.GetPropertyEffectAttributes()->begin(); it != property.GetPropertyEffectAttributes()->end(); ++it) {
            int finalValue = it->second * GetPropertyTotalQuantity();

            if (m_effectAttributes.find(it->first) != m_effectAttributes.end()) {
                finalValue = m_effectAttributes[it->first];
            }
            m_finalEffectAttributes[it->first] = finalValue;
        }
    }

    stShopItemBasicInfo ToBasiclInfo()
    {
        stShopItemBasicInfo basic(m_platform);
        basic.SetShopId(m_shopId);
        basic.SetPropertyId(m_propertyId);
        basic.SetItemId(m_itemId);

        return basic;
    }

    virtual void ToPacket( INetPacket& packet ) 
    {		
        stShopItemBasicInfo::ToPacket(packet);
        packet << m_itemName << m_itemType << m_itemPriceUnit << m_itemPriceValue << m_itemPayCurrency << m_itemPayPoint << m_propertyQuantity << m_propertyGiftQuantity;

        int32 countOfEffectAttributes = m_effectAttributes.size();
        packet << countOfEffectAttributes;
        for (std::map<stPropertyId, int32>::const_iterator it = m_effectAttributes.begin(); it != m_effectAttributes.end(); ++it) {
            packet << (int32)it->first << it->second;
        }
    }

    virtual void PacketTo( INetPacket& packet ) 
    {
        stShopItemBasicInfo::PacketTo(packet);
        packet >> m_itemName >> m_itemType >> m_itemPriceUnit >> m_itemPriceValue >> m_itemPayCurrency >> m_itemPayPoint >> m_propertyQuantity >> m_propertyGiftQuantity;

        int32 countOfEffectAttributes = 0;
        packet >> countOfEffectAttributes;
        for (int i=0; i<countOfEffectAttributes; i++) {
            int32 attr = 0;
            int32 val = 0;
            packet >> attr >> val;
            m_effectAttributes[(stPropertyId)attr] = val;
        }
    }

#ifndef _SERVER_

    virtual void ToStream( FileBuffer& packet ) 
    {		
        stShopItemBasicInfo::ToStream(packet);
        packet << m_itemName << m_itemType << m_itemPriceUnit << m_itemPriceValue << m_itemPayCurrency << m_itemPayPoint << m_propertyQuantity << m_propertyGiftQuantity;

        int32 countOfEffectAttributes = m_effectAttributes.size();
        packet << countOfEffectAttributes;
        for (std::map<stPropertyId, int32>::const_iterator it = m_effectAttributes.begin(); it != m_effectAttributes.end(); ++it) {
            packet << (int32)it->first << it->second;
        }
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        stShopItemBasicInfo::FromStream(packet);
        packet >> m_itemName >> m_itemType >> m_itemPriceUnit >> m_itemPriceValue >> m_itemPayCurrency >> m_itemPayPoint >> m_propertyQuantity >> m_propertyGiftQuantity;

        int32 countOfEffectAttributes = 0;
        packet >> countOfEffectAttributes;
        for (int i=0; i<countOfEffectAttributes; i++) {
            int32 attr = 0;
            int32 val = 0;
            packet >> attr >> val;
            m_effectAttributes[(stPropertyId)attr] = val;
        }
    }

#endif

protected:
    friend class std::map<std::string, stShopItemDetailInfo>;
    friend class ShopItemTable;

    std::string		m_itemName;
    std::string		m_itemType;
    std::string		m_itemPriceUnit;
    float			m_itemPriceValue;
    int32			m_itemPayCurrency;
    int32			m_itemPayPoint;
    int32			m_propertyQuantity;		// 购买数量
    int32			m_propertyGiftQuantity; // 赠送数量
    std::map<stPropertyId, int32> m_effectAttributes;
    std::map<stPropertyId, int32> m_finalEffectAttributes;

};

class ShopItemTable 
    : public std::map<std::string, stShopItemDetailInfo>
{
public:

    stShopItemDetailInfo at(int index)
    {
        iterator it = begin();
        while(index > 0 && it != end())
        {
            --index;
            ++it;
        }
        return it->second;
    }

    const stShopItemDetailInfo at(int index) const
    {
        const_iterator it = begin();
        while(index > 0 && it != end())
        {
            --index;
            ++it;
        }
        return it->second;
    }

    virtual iterator find_by_item_id(const std::string& item_id)
    {
        for (iterator it = begin(); it != end(); ++it)
        {
            if (it->second.GetItemId() == item_id)
            {
                return it;
            }
        }
        return end();
    }

    virtual const_iterator find_by_item_id(const std::string& item_id) const
    {
        for (const_iterator it = begin(); it != end(); ++it)
        {
            if (it->second.GetItemId() == item_id)
            {
                return it;
            }
        }
        return end();
    }

    virtual void ToPacket( INetPacket& packet ) 
    {
        packet << (int32)size();
        for (iterator it = begin(); it != end(); ++it)
        {
            packet << it->first;
            it->second.ToPacket(packet);
        }
    }

    virtual void PacketTo( INetPacket& packet ) 
    {
        int size;
        packet >> size;
        for(int i = 0; i < size; ++i)
        {
            std::string key;
            stShopItemDetailInfo item;
            packet >> key;
            item.PacketTo(packet);
            (*this)[key] = item;
        }
    }

#ifndef _SERVER_
    virtual void ToStream( FileBuffer& packet ) 
    {
        packet << (int32)size();
        for (iterator it = begin(); it != end(); ++it)
        {
            packet << it->first;
            it->second.ToStream(packet);
        }
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        int size;
        packet >> size;
        for(int i = 0; i < size; ++i)
        {
            std::string key;
            stShopItemDetailInfo item;
            packet >> key;
            item.FromStream(packet);
            (*this)[key] = item;
        }
    }
#endif

};

class PropertyLibrary : public std::map<stPropertyId, stProperty> {
public:
    PropertyLibrary() : m_version(-1) {}
    virtual ~PropertyLibrary() {}

public:

    PropertyVersion GetVersion() const { return m_version; }
    void SetVersion(PropertyVersion val) { m_version = val; }

    virtual void ToPacket( INetPacket& packet ) 
    {
        packet << m_version;

        int32 countOfProperty = this->size();
        packet << countOfProperty;
        for (PropertyLibrary::iterator it = this->begin(); it != this->end(); ++it) {
            packet << it->first;
            it->second.ToPacket(packet);
        }
    }

    virtual void PacketTo( INetPacket& packet ) 
    {
        packet >> m_version;

        int32 countOfProperty = 0;
        packet >> countOfProperty;
        for (int i=0; i<countOfProperty; i++) {
            stPropertyId id;
            packet >> id;
            stProperty & prop = (*this)[id];
            prop.PacketTo(packet);
        }
    }

#ifndef _SERVER_
    virtual void ToStream( FileBuffer& packet )
    {
        packet << m_version;

        int32 countOfProperty = this->size();
        packet << countOfProperty;
        for (PropertyLibrary::iterator it = this->begin(); it != this->end(); ++it) {
            packet << it->first;
            it->second.ToStream(packet);
        }
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        packet >> m_version;

        int32 countOfProperty = 0;
        packet >> countOfProperty;
        for (int i=0; i<countOfProperty; i++) {
            stPropertyId id;
            packet >> id;
            stProperty & prop = (*this)[id];
            prop.FromStream(packet);
        }
    }
#endif

protected:
    PropertyVersion m_version;
};

class ShopLibrary 
    : public std::map<stPropertyId, ShopItemTable>
{
public:
    virtual void ToPacket( INetPacket& packet ) 
    {
        packet << (int32)size();
        for (iterator it = begin(); it != end(); ++it)
        {		
            packet << it->first;
            it->second.ToPacket(packet);
        }
    }

    virtual void PacketTo( INetPacket& packet ) 
    {
        int size;
        packet >> size;
        for(int i = 0; i < size; ++i)
        {
            stPropertyId id;
            ShopItemTable item;
            packet >> id;
            item.PacketTo(packet);
            (*this)[id] = item;
        }
    }

#ifndef _SERVER_
    virtual void ToStream( FileBuffer& packet ) 
    {
        packet << (int32)size();
        for (iterator it = begin(); it != end(); ++it)
        {
            packet << it->first;
            it->second.ToStream(packet);
        }
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        int size;
        packet >> size;
        for(int i = 0; i < size; ++i)
        {
            stPropertyId id;
            ShopItemTable item;
            packet >> id;
            item.FromStream(packet);
            (*this)[id] = item;
        }
    }
#endif

};

typedef ShopLibrary::iterator ShopLibraryIter;
typedef ShopItemTable::iterator ShopItemTableIter;

typedef std::vector<stShopItemDetailInfo>			ShopItemList;
typedef std::map<stPropertyId, ShopItemList>	 ShopTable;

#ifdef _SERVER_
typedef std::vector<stShopItemDetailInfo>		ShopItems;
typedef std::map<stPropertyId, ShopItems>	ShopDB;
#endif

struct stShopUpdate {
    int						version;
    PaymentServicePlatform	provider;
    ShopLibrary				updates;
    bool					purge;


	stShopUpdate() : version(0), purge(false), provider(InvalidPlatform)
	{}

    virtual void ToPacket( INetPacket& packet ) 
    {
        packet << version << (int)provider << purge;
        updates.ToPacket(packet);
    }

    virtual void PacketTo( INetPacket& packet ) 
    {
        int iProvider;
        packet >> version >> iProvider >> purge;
        provider = PaymentServicePlatform(iProvider);
        updates.PacketTo(packet);
    }

#ifndef _SERVER_
    virtual void ToStream( FileBuffer& packet ) 
    {
        packet << version << (int)provider << purge;
        updates.ToStream(packet);
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        int iProvider;
        packet >> version >> iProvider >> purge;
        provider = PaymentServicePlatform(iProvider);
        updates.FromStream(packet);
    }
#endif

};

//首次充值
struct stShopDiscount
{
    enum ShopDiscountType {//1 打折 2送
        Shop_Type_Dscount = 1,
        Shop_Type_Handsel,
    };

    enum ShopDiscountUnit {//1：百分比 2:整数
        Shop_Unit_Percentage = 1,
        Shop_Unit_Numerical,
    };

    enum ShopDiscountCategory {
        SHOP_DISCOUNT_CATEGORY_FIRST_PAYMENT = 1,	 // 首充优惠
        SHOP_DISCOUNT_CATEGORY_ACTIVITY = 2,		 // 活动优惠
    };

    //商品id，""对应所有商品
    std::string strGameItemID;	
    std::string strMsg;
    int			nType;			//1 打折 2送
    int			nValue;			//
    int			nUnit;			//1：百分比 2:整数
    int			category;		// 优惠类别

    stShopDiscount()
        : strGameItemID(""), strMsg(""), nType(0), nUnit(0), category(0)
    {
    }

    void ToPacket(INetPacket & pack)
    {
        pack<< strGameItemID << strMsg << nType << nValue << nUnit << category;
    }

    void PacketTo(INetPacket & pack)
    {
        pack>> strGameItemID >> strMsg >> nType >> nValue >> nUnit >> category;
    }

    bool IsForAllItem()
    {
        return (strMsg.length()==0);
    }

    uint32 ValueChange(uint32 num) const
    {
        if(nUnit == Shop_Unit_Percentage)
        {
            num += num*nValue/100;
        }
        else if (nUnit == Shop_Unit_Numerical)
        {
            num += nValue;
        }

        return num;
    }
};

typedef std::vector<stShopDiscount>	stShopDiscountList;


#ifndef _SERVER_

struct OrderCache
{
    std::string		platform;
    std::string		shopId;
    int32       	propertyId;
    std::string		itemId;
    int32			quantity;
    std::string		orderId;
    std::string		receipt;
    int32			result;

    virtual void ToStream( FileBuffer& packet ) 
    {
        packet << platform << shopId << propertyId << itemId << quantity << orderId << receipt << result;
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        packet >> platform >> shopId >> propertyId >> itemId >> quantity >> orderId >> receipt >> result;
    }
};

class ShopCompleteOrderCache 
    : public std::map<std::string, OrderCache>
{
public:

#ifndef _SERVER_
    virtual void ToStream( FileBuffer& packet ) 
    {
        packet << (int32)size();
        for (iterator it = begin(); it != end(); ++it)
        {
            packet << it->first;
            it->second.ToStream(packet);
        }
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        int size;
        packet >> size;
        for(int i = 0; i < size; ++i)
        {
            std::string id;
            OrderCache item;
            packet >> id;
            item.FromStream(packet);
            (*this)[id] = item;
        }
    }
#endif

};

void LoadCompleteOrderCache();
void SaveCompleteOrderCahce(const std::string& orderId, const OrderCache& completeOrder);
void RemoveCompleteOrderCache(const std::string& orderId);
ShopCompleteOrderCache GetShopCompleteOrderCache();

struct ShopCache {
    int			version;
    ShopLibrary	table;

    ShopCache()
        : version(0)
    {
    }

    virtual void ToStream( FileBuffer& packet ) 
    {
        packet << version;
        table.ToStream(packet);
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        packet >> version;
        table.FromStream(packet);
    }
};

typedef std::map<PaymentServicePlatform, ShopCache>	ShopStore;
typedef ShopStore::iterator	ShopStoreIter;

struct PropertyCache {
    PropertyLibrary	properties;

    PropertyCache()
    {
    }

    virtual void ToStream( FileBuffer& packet ) 
    {
        properties.ToStream(packet);
    }

    virtual void FromStream( FileBuffer& packet ) 
    {
        properties.FromStream(packet);
    }
};

bool LoadShopTable(PaymentServicePlatform provider, int& version);
bool SaveShopTable(PaymentServicePlatform provider, int version, const ShopLibrary& library);
bool DeleteShopTable(PaymentServicePlatform provider);
const ShopLibrary& GetShopTable(PaymentServicePlatform provider = MyGame);
bool GetShopItem(PaymentServicePlatform provider, const stPropertyId& propertyId, const std::string& gameItemId, stShopItemDetailInfo& info);
bool GetPlatformItem(PaymentServicePlatform provider, const stPropertyId& propertyId, const std::string& itemId, stShopItemDetailInfo& info);

bool LoadProperty(PropertyLibrary &properties);
bool SaveProperty(const PropertyLibrary& properties);
bool DeleteProperty();

struct PaymentServicePayOrderParams;
void UpdateOrderParams( PaymentServicePlatform provider, PaymentServicePayOrderParams * pParams );

typedef std::vector<PaymentServicePlatform> SupportedPlatforms;
void GetSupportedPlatforms(SupportedPlatforms& lists);
/*
除MyGame外，其他支持的支付方式
*/
bool GetSupportedPlatformsBy(const stPropertyId& properyId, const std::string& gameItemId, SupportedPlatforms& lists);

void SetShopDiscountList(stShopDiscountList& lists);
bool GetShopDiscount(int category, stShopDiscountList& discount);
void GetSortShopItemList(const PaymentServicePlatform provider, const enum ShopPayPointType paypoint, const stPropertyId& propertyId, ShopItemList &lists);

void SetCurrentPayPoint(const enum ShopPayPointType paypoint = SHOP_PAY_POINT_SHOP);
ShopPayPointType GetCurrentPayPoint();


struct BestChargeItemContext;

typedef bool (*ShopItemPredictor)( BestChargeItemContext& context, const stShopItemDetailInfo& info);
typedef int (*ShopItemCostEvaluator)( const BestChargeItemContext& context, const stShopItemDetailInfo& info);

bool FindBestChargeItem( BestChargeItemContext& context,  stShopItemDetailInfo& info );
int StandardCostEvaluator( const BestChargeItemContext& context, const stShopItemDetailInfo& info);

bool StandardCoinsChargeItemPredictor( BestChargeItemContext& context, const stShopItemDetailInfo& info);
bool StandardDiamondsChargeItemPredictor( BestChargeItemContext& context, const stShopItemDetailInfo& info);

struct BestChargeItemContext {
    int					paymentPoints;
    int					paymentMethods;
    int					chargeValue;
    int					chargeQuantity;
    ShopItemPredictor	itemPredictor;
    ShopItemCostEvaluator
        costEvaluator;

    BestChargeItemContext();
};


struct PaymentServiceLoginResult {
    PaymentServicePlatform	provider;	
    bool					success;
    bool					bNewLogin;
    bool					bShouldModifyAccountInfo;
    std::string				uid;
    std::string				userName;
    std::string				nickName;
    int						sex;		// 0 = 女，1 = 男
    int						icon;
    uint32					birthday;	// 生日的 Unix 时间戳

    PaymentServiceLoginResult()
        : provider(InvalidPlatform)
        , success(false)
        , bNewLogin(false)
        , sex(0)
        , birthday(0)
        , bShouldModifyAccountInfo(false)
    {
    }
};

struct PaymentServiceOrderResult {

    enum {
        OK,
        Failed,
        Cancelled,
        Unspecified,

        StatusMask = 0x00FFFFFF,
        Pending = 0x01000000,
        FlagMask = 0xFF000000,
    };

    PaymentServicePlatform	provider;
    int						result;

    std::string				orderId;
    std::string				receipt;
    PaymentServiceOrderResult()
        : provider(InvalidPlatform)
        , result(Unspecified)
    {
    }

    bool IsCancelled() const {
        return Cancelled == (StatusMask & result);
    }

    bool IsSuccess() const {
        return OK == (StatusMask & result);
    }

    bool IsFailed() const {
        return Failed == (StatusMask & result);
    }

    bool NeedPendingAction() const {
        return Pending == (FlagMask & result);
    }

};


struct PaymentServiceParams {
    void * pContext;
};


struct PaymentServiceLoginParams 
    : public PaymentServiceParams {
        PaymentServicePlatform provider;
};

struct PaymentServiceCommunityParams 
    : public PaymentServiceParams {
        PaymentServicePlatform provider;
};

struct PaymentServicePayOrderParams 
    : public PaymentServiceParams {
        std::string		orderId;
        std::string		gameId;			// 游戏的 ID
        stPropertyId		propertyId;	// 游戏内的分类 ID
        std::string		gameItemId;		// 游戏内的商品 ID
        PaymentServicePlatform provider;
        std::string     userId;
        std::string		shopId;
        std::string		itemId;
        std::string		itemName;
        float			price;
        int			    currency;
        int				quantity;
        bool acceptsQuantity;
        std::string     extendStr;
        PaymentServicePayOrderParams()
            : price(0.f)
            , quantity(0)
            , propertyId(0)
            , acceptsQuantity(true)
        {
        }
};

typedef std::vector<PaymentServicePayOrderParams> PayOrderList;

typedef void (*PaymentServiceLoginResultCallback)(PaymentServiceLoginParams *, const PaymentServiceLoginResult * );
typedef void (*PaymentServiceCommunityCallback)(PaymentServiceCommunityParams *, const PaymentServiceLoginResult * );
typedef void (*PaymentServiceOrderResultCallback)(PaymentServicePayOrderParams *, const PaymentServiceOrderResult * );

void RequstPaymentServiceLogin(PaymentServicePlatform provider, PaymentServiceLoginParams * pParams, PaymentServiceLoginResultCallback callback);
void RequstPaymentServiceCommunity(PaymentServicePlatform provider, PaymentServiceCommunityParams * pParams, PaymentServiceCommunityCallback callback);
void RequstPaymentServicePayForOrder(PaymentServicePlatform provider, PaymentServicePayOrderParams * pParams, PaymentServiceOrderResultCallback callback);
const char* RequestPaymentExtendStr(PaymentServicePlatform provider);
void RequstPaymentServiceDismiss();

#endif

