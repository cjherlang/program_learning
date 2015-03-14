#include "GameBasePCH.h"
#include "ShopDef.h"
#include "OS.h"

#ifdef _SERVER_
#include "platform.h"
#endif

#if ((defined IOS) || (defined ANDROID))
#ifdef _SERVER_
#include "engine/System.h"
#else
#include "System.h"
#endif
#endif

#ifndef _SERVER_
#pragma execution_character_set("utf-8")
#endif

BEGIN_ENUM_HELPER(PaymentServicePlatform)
    ENUM_ITEM_TEXT_EX(Diandian, "diandian", "点点支付", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(NdStore, "91", "91支付平台", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(MyGame, "MyGame", "游戏支付平台", PAYMENT_METHOD_ALL),
#if defined(IOS) || defined(_SERVER_) || defined(_WIN32)
    ENUM_ITEM_TEXT_EX(AppStore, "app_store", "App Store支付平台", PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(TbStore, "tongbutui", "同步推支付", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(KYStore, "KYStore", "快用支付", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(PPStore, "PPStore", "PP支付", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
#endif

    ENUM_ITEM_TEXT_EX(WpayStore, "WpayStore", "微派支付", PAYMENT_METHOD_SMS),
    ENUM_ITEM_TEXT_EX(MMBilling, "MMBilling", "移动话费支付", PAYMENT_METHOD_SMS),
    ENUM_ITEM_TEXT_EX(CTEStore, "CTEStore", "电信话费支付", PAYMENT_METHOD_SMS),
    ENUM_ITEM_TEXT_EX(UniconStore, "UniconStore", "联通话费支付", PAYMENT_METHOD_SMS),
    ENUM_ITEM_TEXT_EX(MMSMSBilling, "MMSMSBilling", "移动话费支付", PAYMENT_METHOD_SMS),
    ENUM_ITEM_TEXT_EX(QIHUBilling, "QIHUBilling", "360支付", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(G3Billing, "G3Billing", "3g门户支付", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(LenovoStore, "LenovoStore", "联想支付", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(UniPay, "UniPay", "联通话费支付", PAYMENT_METHOD_SMS),
    ENUM_ITEM_TEXT_EX(BaiduStore, "BaiduStore", "百度商店", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(XiaomiPay, "XiaomiPay", "小米支付", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(HuaweiPay, "HuaweiPay", "华为支付",PAYMENT_METHOD_SMS | PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(OppoPay, "OppoPay", "oppo支付", PAYMENT_METHOD_SMS | PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(DJPay, "DJPay", "点金支付", PAYMENT_METHOD_SMS | PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(WNPay, "WNPay", "蜗牛支付", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),

    //local use for dpay ,new item should be put ahead 
    ENUM_ITEM_TEXT_EX(AliPay, "AliPay", "支付宝", PAYMENT_METHOD_BANK),
    ENUM_ITEM_TEXT_EX(TenPay, "TenPay", "财付通", PAYMENT_METHOD_BANK),
    ENUM_ITEM_TEXT_EX(PhCardPay, "PhCardPay", "电话卡支付", PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(GCardPay, "GCardPay", "游戏卡支付", PAYMENT_METHOD_PREPAID_CARD),
    ENUM_ITEM_TEXT_EX(DefPay, "DefPay", "其他支付", PAYMENT_METHOD_BANK | PAYMENT_METHOD_PREPAID_CARD),
END_ENUM_HELPER(PaymentServicePlatform)


BEGIN_ENUM_HELPER(PropertyType)
    ENUM_ITEM_TEXT(PROPERTY_GOLD_COINS, "::coin::", "金币"),
    ENUM_ITEM_TEXT(PROPERTY_GOLD_NOTES, "::note::", "金券"),
    ENUM_ITEM_TEXT(PROPERTY_DIAMONDS, "::dianmond::", "钻石"),
    ENUM_ITEM_TEXT(PROPERTY_HORN, "::horn::", "小喇叭"),
    ENUM_ITEM_TEXT(PROPERTY_TOTAL_CHARGE_FEE, "::coin::", "金币"),
    ENUM_ITEM_TEXT(PROPERTY_TICKET, "::ticket::", "抽奖券"),
    ENUM_ITEM_TEXT(PROPERTY_COUPON, "::coupon::", "兑换券"),
    ENUM_ITEM_TEXT(PROPERTY_MATCH_NOTES, "::matchnote::", "参赛券"),
	ENUM_ITEM_TEXT(LITTLE_PAY, "::littlepay::", "小额支付"),
    ENUM_ITEM_TEXT(PROPERTY_KICK_CARD, "::kickcard::", "踢人卡"),

END_ENUM_HELPER(PropertyType)

BEGIN_ENUM_HELPER(PaymentCurrencyType)
    ENUM_ITEM_TEXT(PAYMENT_CURRENCY_MONEY, "￥", "现金"),
    ENUM_ITEM_TEXT(PAYMENT_CURRENCY_GOLDS, "::coin::", "金币"),
END_ENUM_HELPER(PaymentCurrencyType)

static const std::string STR_SHOP_PAY_POINT_SHOP = "|shop|";
static const std::string STR_SHOP_PAY_POINT_SUPER_SHOP = "|super_shop|";
static const std::string STR_SHOP_PAY_POINT_CHANGE_CARD = "|change_card|";
static const std::string STR_SHOP_PAY_POINT_PROPERTY_SHOP = "|property_shop|";

static const std::string STR_PAYMENT_CURRENCY_MONEY = "money";
static const std::string STR_PAYMENT_CURRENCY_GOLDS = "golds";

stPropertyId stProperty::GetPropertyId(const std::string &p) {
	if (p == "Golds") {
		return PROPERTY_GOLD_COINS;
	} else if (p == "Diamonds") {
		return PROPERTY_DIAMONDS;
	} else if (p == "Notes") {
		return PROPERTY_GOLD_NOTES;
	} else if (p == "Horn") {
		return PROPERTY_HORN;
	} else if (p == "MatchNote") {
		return PROPERTY_MATCH_NOTES;
	}else if (p == "LittlePay"){
		return PROPERTY_GOLD_COINS;
	}else if (p == "KickCard")
	{
		return PROPERTY_KICK_CARD;
	}
	
	assert(false);
	return -1;
}

stShopItemBasicInfo::stShopItemBasicInfo(const std::string& platform)
: m_platform(platform), m_paymentMethod(GetPaymentServicePlatformExtraValueByKey(platform.c_str()))
{
}

stShopItemBasicInfo::stShopItemBasicInfo(PaymentServicePlatform platform)
: m_platform(GetPaymentServicePlatformKey(platform)), m_paymentMethod(GetPaymentServicePlatformExtraValue(platform))
{

}

int32 stShopItemBasicInfo::GetPaymentMethod() const {
    return m_paymentMethod;
}

bool stShopItemBasicInfo::ContainsPaymentMethod( int paymentMethods ) const
{
    return (m_paymentMethod & paymentMethods) > 0;
}

bool stShopItemBasicInfo::AcceptsQuantity() const {
    return m_acceptQuantity > 1;
}

void stShopItemDetailInfo::SetItemPayPoint(std::string &val) {
	int v = 0;
	std::string formattedVal = "|" + val + "|";
	if (formattedVal.find(STR_SHOP_PAY_POINT_SHOP) != std::string::npos) {
		v |= SHOP_PAY_POINT_SHOP;
	}
	if (formattedVal.find(STR_SHOP_PAY_POINT_CHANGE_CARD) != std::string::npos) {
		v |= SHOP_PAY_POINT_CHANGE_CARD;
	}
	if (formattedVal.find(STR_SHOP_PAY_POINT_PROPERTY_SHOP) != std::string::npos) {
		v |= SHOP_PAY_POINT_PROPERTY_SHOP;
	}
	if (formattedVal.find(STR_SHOP_PAY_POINT_SUPER_SHOP) != std::string::npos){
		v |= SHOP_PAY_POINT_SUPER_SHOP;
	}
	m_itemPayPoint = v;
}

bool stShopItemDetailInfo::ContainsEntryPoint( int entryPoints ) const
{
    return (m_itemPayPoint & entryPoints) > 0;
}

void stShopItemDetailInfo::SetItemPayCurrency(std::string &val) {
    int v = 0;
    if (val == STR_PAYMENT_CURRENCY_MONEY) {
        v = PAYMENT_CURRENCY_MONEY;
    } else if (val == STR_PAYMENT_CURRENCY_GOLDS) {
        v = PAYMENT_CURRENCY_GOLDS;
    }
    m_itemPayCurrency = v;
}

int32 stShopItemDetailInfo::GetFinalProperyEffectAttribute( stPropertyId attr, int32 defaultValue /*= 0*/ ) const
{
    std::map<stPropertyId, int32>::const_iterator it = m_finalEffectAttributes.find(attr);
    if (it != m_finalEffectAttributes.end())
    {
        return it->second;
    }
    return defaultValue;
}

int32 stShopItemDetailInfo::GetProperyEffectAttribute( stPropertyId attr, int32 defaultValue /*= 0*/ ) const
{
    std::map<stPropertyId, int32>::const_iterator it = m_effectAttributes.find(attr);
    if (it != m_effectAttributes.end())
    {
        return it->second;
    }
    return defaultValue;
}


#ifndef _SERVER_

#include <fstream>

#define SHOP_COMPLETE_ORDER_CACHE_MGIC 0x7344

ShopCompleteOrderCache g_CompleteOrderCache;

void LoadCompleteOrderCache()
{
    std::string dir;
    std::string cacehFile;

#if ((defined IOS) || (defined ANDROID))
    dir = System::GetCachePath("Shop");
    cacehFile = dir + "/" + "complete_order.db";
#else
    dir = OS::GetCachePath() + "\\Shop";

    cacehFile = dir + "\\" + "complete_order.db";
#endif
    FileBuffer fs(cacehFile.c_str(), "rb");
    bool loaded = false;	
    try
    {
        do 
        {
            if (!fs.IsOpened())
                break;
            int magic;
            fs >> magic;
            if (magic != SHOP_COMPLETE_ORDER_CACHE_MGIC)
                break;
            g_CompleteOrderCache.FromStream(fs);
            loaded = true;
        } while (0);
    }
    catch (...)
    {
        loaded = false;
    }
    if (fs.IsOpened())
    {
        fs.Close();
    }
    if (!loaded)
    {
        g_CompleteOrderCache.clear();
    }
}

void SaveCompleteOrderCache()
{
    std::string dir;
    std::string cacehFile;
#if ((defined IOS) || (defined ANDROID))
    dir = System::GetCachePath("Shop");
    System::CreateFile(dir.c_str(), true);
    cacehFile = dir + "\\" + "complete_order.db";
#else
    dir = OS::GetCachePath() + "\\Shop";
    OS::CreateDirectory(dir, true);
    cacehFile = dir + "\\" + "complete_order.db";
#endif
    FileBuffer fs(cacehFile.c_str(), "wb");
    do 
    {
        if (!fs.IsOpened())
            break;
        fs << SHOP_COMPLETE_ORDER_CACHE_MGIC;
        g_CompleteOrderCache.ToStream(fs);
        fs.Flush();
    } while (0);
    if (fs.IsOpened())
    {
        fs.Close();
    }
}

void SaveCompleteOrderCahce(const std::string& orderId, const OrderCache& completeOrder)
{
    if (orderId.empty() == false)
    {
        ShopCompleteOrderCache::iterator it = g_CompleteOrderCache.find(orderId);
        //if orderId in cache ,do not add it
        if (it == g_CompleteOrderCache.end())
        {
            g_CompleteOrderCache[orderId] = completeOrder;
            SaveCompleteOrderCache();
        }
    }
}

void DeleteShopCompleteOrderCache()
{
    std::string dir;
    std::string cacehFile;
#if ((defined IOS) || (defined ANDROID))
    dir = System::GetCachePath("Shop");
    System::CreateFile(dir.c_str(), true);
    cacehFile = dir + "\\" + "complete_order.db";
    System::DeleteFilePath(cacehFile.c_str());
#else
    dir = OS::GetCachePath() + "\\Shop";
    OS::CreateDirectory(dir, true);
    cacehFile = dir + "\\" + "complete_order.db";
    //TODO
#endif
}

void RemoveCompleteOrderCache(const std::string& orderId)
{
    if (orderId.empty() == false)
    {
        ShopCompleteOrderCache::iterator it = g_CompleteOrderCache.begin();
        while(it != g_CompleteOrderCache.end())
        {
            if (orderId == it->first)
            {
                g_CompleteOrderCache.erase(it++);
                continue;
            }
            it++;
        }
        if (g_CompleteOrderCache.size() <= 0)
        {
            DeleteShopCompleteOrderCache();
        } 
        else
        {
            SaveCompleteOrderCache();
        }
    }
}

ShopCompleteOrderCache GetShopCompleteOrderCache()
{
    if (g_CompleteOrderCache.size() <= 0)
    {
        LoadCompleteOrderCache();
    }
    return g_CompleteOrderCache;
}

#define SHOP_TABLE_CACHE_MAGIC	0x7344CA09

ShopStore	g_ShopStore;
PropertyLibrary g_PropertyLibrary;

stShopDiscountList g_Discount;

ShopPayPointType g_CurrentPayPoint;

void LoadShopTableCache( PaymentServicePlatform provider, ShopCache& cache ) 
{
    std::string dir;
    std::string cacehFile;

#if ((defined IOS) || (defined ANDROID))
    dir = System::GetCachePath("Shop");
    cacehFile = dir + "/" + GetPaymentServicePlatformKey(provider) + ".db";
#else
    dir = OS::GetCachePath() + "\\Shop";

    cacehFile = dir + "\\" + GetPaymentServicePlatformKey(provider) + ".db";
#endif
    FileBuffer fs(cacehFile.c_str(), "rb");
    bool loaded = false;	
    try
    {
        do 
        {
            if (!fs.IsOpened())
                break;
            int magic;
            fs >> magic;
            if (magic != SHOP_TABLE_CACHE_MAGIC)
                break;
            cache.FromStream(fs);
            loaded = true;
        } while (0);
    }
    catch (...)
    {
        loaded = false;
    }
    if (fs.IsOpened())
    {
        fs.Close();
    }
    if (!loaded)
    {
        cache.version = 0;
        cache.table.clear();
    }
}


void SaveShopTableCache( PaymentServicePlatform provider, ShopCache& cache )
{
    std::string dir;
    std::string cacehFile;
#if ((defined IOS) || (defined ANDROID))
    dir = System::GetCachePath("Shop");
    System::CreateFile(dir.c_str(), true);
    cacehFile = dir + "/" + GetPaymentServicePlatformKey(provider) + ".db";
#else
    dir = OS::GetCachePath() + "\\Shop";
    OS::CreateDirectory(dir, true);
    cacehFile = dir + "\\" + GetPaymentServicePlatformKey(provider) + ".db";
#endif
    FileBuffer fs(cacehFile.c_str(), "wb");
    do 
    {
        if (!fs.IsOpened())
            break;
        fs << SHOP_TABLE_CACHE_MAGIC;
        cache.ToStream(fs);
        fs.Flush();
    } while (0);
    if (fs.IsOpened())
    {
        fs.Close();
    }
}

void DeleteShopTableCache(PaymentServicePlatform provider)
{
    std::string dir;
    std::string cacehFile;
#if ((defined IOS) || (defined ANDROID))
    dir = System::GetCachePath("Shop");
    System::CreateFile(dir.c_str(), true);
    cacehFile = dir + "/" + GetPaymentServicePlatformKey(provider) + ".db";
    System::DeleteFilePath(cacehFile.c_str());
#else
    dir = OS::GetCachePath() + "\\Shop";
    OS::CreateDirectory(dir, true);
    cacehFile = dir + "\\" + GetPaymentServicePlatformKey(provider) + ".db";
#endif
}

bool LoadShopTable( PaymentServicePlatform provider, int& version )
{
    ShopStoreIter it = g_ShopStore.find(provider);
    if (g_ShopStore.end() == it)
    {
        ShopCache cache;
        LoadShopTableCache(provider, cache);

        // Merge Effect Property Attribute.
        PropertyLibrary propLib;
        if (LoadProperty(propLib) == false) {
            return false;
        }

        for (ShopLibrary::iterator it = cache.table.begin(); it != cache.table.end(); ++it) {
            stPropertyId propertyId = it->first;
            stProperty &property = propLib[propertyId];
            for (ShopItemTable::iterator sit = it->second.begin(); sit != it->second.end(); ++sit) {
                sit->second.MergePropertyEffectAttributes(property);
            }
        }

        g_ShopStore[provider] = cache;
        version = cache.version;
    }
    else
    {
        version = it->second.version;
    }
    return version > 0;
}

bool SaveShopTable( PaymentServicePlatform provider, int version, const ShopLibrary& library )
{
    // Merge Effect Property Attribute.
    PropertyLibrary propLib;
    if (LoadProperty(propLib) == false) {
        return false;
    }

    ShopLibrary newShopLib = library;
    for (ShopLibrary::iterator it = newShopLib.begin(); it != newShopLib.end(); ++it) {
        stPropertyId propertyId = it->first;
        stProperty &property = propLib[propertyId];
        for (ShopItemTable::iterator sit = it->second.begin(); sit != it->second.end(); ++sit) {
            sit->second.MergePropertyEffectAttributes(property);
        }
    }

    ShopStoreIter it = g_ShopStore.find(provider);
    if (g_ShopStore.end() == it)
    {
        ShopCache cache;
        cache.version = version;
        cache.table = newShopLib;
        SaveShopTableCache(provider, cache);
        g_ShopStore[provider] = cache;
    }
    else
    {
        it->second.version = version;
        it->second.table = newShopLib;
        SaveShopTableCache(provider, it->second);
    }
    return false;
}

bool DeleteShopTable(PaymentServicePlatform provider)
{
    ShopStoreIter it = g_ShopStore.find(provider);
    if (g_ShopStore.end() == it)
    {
        return false;
    }
    else
    {
        DeleteShopTableCache(provider);
        g_ShopStore.erase(it);
        return true;
    }
}

bool LoadProperty(PropertyLibrary &properties) {
    if (g_PropertyLibrary.GetVersion() != -1) {
        properties = g_PropertyLibrary;
        return true;
    }

    std::string dir;
    std::string cacehFile;
#if ((defined IOS) || (defined ANDROID))
    dir = System::GetCachePath("Property");
    cacehFile = dir + "/Property.db";
#else
    dir = OS::GetCachePath() + "\\Shop";
    cacehFile = dir + "\\Property.db";
#endif
    FileBuffer fs(cacehFile.c_str(), "rb");
    bool loaded = false;	
    try
    {
        do 
        {
            if (!fs.IsOpened())
                break;
            int magic;
            fs >> magic;
            if (magic != SHOP_TABLE_CACHE_MAGIC)
                break;
            properties.FromStream(fs);
            g_PropertyLibrary = properties;
            loaded = true;
        } while (0);
    }
    catch (...)
    {
        loaded = false;
    }
    if (fs.IsOpened())
    {
        fs.Close();
    }
    return loaded;
}

bool SaveProperty(const PropertyLibrary& properties) {
    g_PropertyLibrary = properties;

    std::string dir;
    std::string cacehFile;
    bool ret = true;

#if ((defined IOS) || (defined ANDROID))
    dir = System::GetCachePath("Property");
    System::CreateFile(dir.c_str(), true);
    cacehFile = dir + "/Property.db";
#else
    dir = OS::GetCachePath() + "\\Shop";
    OS::CreateDirectory(dir, true);
    cacehFile = dir + "\\Property.db";
#endif
    FileBuffer fs(cacehFile.c_str(), "wb");
    do 
    {
        if (!fs.IsOpened()) {
            ret = false;
            break;
        }
        fs << SHOP_TABLE_CACHE_MAGIC;
        PropertyLibrary *p = (PropertyLibrary*)&properties;
        p->ToStream(fs);
        fs.Flush();
    } while (0);
    if (fs.IsOpened())
    {
        fs.Close();
    }
    return ret;
}

bool DeleteProperty() {
    std::string dir;
    std::string cacehFile;
    bool ret = true;
#if ((defined IOS) || (defined ANDROID))
    dir = System::GetCachePath("Property");
    cacehFile = dir + "/Property.db";
#else
    dir = OS::GetCachePath() + "\\Shop";
    cacehFile = dir + "\\Property.db";
#endif

    unlink(cacehFile.c_str());

    g_PropertyLibrary.clear();

    return ret;
}


//get by provider/ gameCategoryId /gameItemId
bool GetShopItem( PaymentServicePlatform provider, const stPropertyId& propertyId, const std::string& gameItemId, stShopItemDetailInfo& info )
{
    int version;
    LoadShopTable(provider, version);
    ShopLibrary& table = g_ShopStore[provider].table;
    ShopLibrary::iterator catIt = table.find(propertyId);
    if (catIt != table.end())
    {
        ShopItemTable& itemTable = catIt->second;
        ShopItemTable::iterator itemIt = itemTable.find(gameItemId);
        if (itemIt != itemTable.end())
        {
            info = itemIt->second;
            return true;
        }
    }
    return false;
}

//get by provider/ categoryId / itemId
bool GetPlatformItem(PaymentServicePlatform provider, const stPropertyId& propertyId, const std::string& itemId, stShopItemDetailInfo& info)
{
    int version;
    LoadShopTable(provider, version);
    ShopLibrary& table = g_ShopStore[provider].table;
    ShopLibrary::iterator catIt = table.find(propertyId);
    if (catIt != table.end())
    {
        ShopItemTable& itemTable = catIt->second;
        ShopItemTable::iterator itemIt = itemTable.find_by_item_id(itemId);
        if (itemIt != itemTable.end())
        {
            info = itemIt->second;
            return true;
        }
    }
    return false;
}

const ShopLibrary& GetShopTable( PaymentServicePlatform provider /*= MyGame*/ )
{
    return g_ShopStore[provider].table;
}


typedef void (*PaymentLogin)(PaymentServiceLoginParams *, PaymentServiceLoginResultCallback);
typedef void (*PaymentPayOrder)(PaymentServicePayOrderParams *, PaymentServiceOrderResultCallback);
typedef void (*PaymentCommunity)(PaymentServiceCommunityParams *, PaymentServiceCommunityCallback);


#define DECLARE_PAYMENT_HANDLER(provider)	void provider##Login(PaymentServiceLoginParams *, PaymentServiceLoginResultCallback); \
    void provider##PayOrder(PaymentServicePayOrderParams *, PaymentServiceOrderResultCallback); \
    void provider##Community(PaymentServiceCommunityParams *, PaymentServiceCommunityCallback)

DECLARE_PAYMENT_HANDLER(Platform);

//std::string getPaymentExtendStr(PaymentServicePlatform provider);

void RequstPaymentServiceLogin( PaymentServicePlatform provider, PaymentServiceLoginParams * pParams, PaymentServiceLoginResultCallback callback )
{
    PlatformLogin(pParams, callback);
}

void UpdateOrderParams( PaymentServicePlatform provider, PaymentServicePayOrderParams * pParams );

void RequstPaymentServicePayForOrder( PaymentServicePlatform provider, PaymentServicePayOrderParams * pParams, PaymentServiceOrderResultCallback callback )
{
    if(provider != InvalidPlatform && provider != MyGame)
    {
        UpdateOrderParams(provider, pParams);
    }

    PlatformPayOrder(pParams, callback);
}

void RequstPaymentServiceCommunity( PaymentServicePlatform provider, PaymentServiceCommunityParams * pParams, PaymentServiceCommunityCallback callback )
{
    PlatformCommunity(pParams, callback);
}

const char* RequestPaymentExtendStr(PaymentServicePlatform provider)
{
    std::string extendStr = "";//getPaymentExtendStr(provider);
    return extendStr.c_str();
}

void UpdateOrderParams( PaymentServicePlatform provider, PaymentServicePayOrderParams * pParams )
{
    if(InvalidPlatform == provider || MyGame == provider)
    {
        return;
    }
    stShopItemDetailInfo item(provider); 
    if(GetShopItem(provider, pParams->propertyId, pParams->gameItemId, item))
    {
        pParams->provider = provider;
        pParams->shopId = item.GetShopId();
        pParams->itemName = item.GetItemName();
        pParams->propertyId = item.GetPropertyId();
        pParams->itemId = item.GetItemId();
        pParams->price = item.GetItemPriceValue();
    }
}

void GetSupportedPlatforms(SupportedPlatforms& lists)
{
    ShopStoreIter it = g_ShopStore.begin();
    while(g_ShopStore.end() != it)
    {
        PaymentServicePlatform provider = it->first;
        lists.push_back(provider);
        it ++;
    }
}

bool GetSupportedPlatformsBy(const stPropertyId& propertyId, const std::string& gameItemId, SupportedPlatforms& lists)
{
    bool isSupported = false;
    for(ShopStoreIter it = g_ShopStore.begin(); it != g_ShopStore.end(); ++it)
    {
        PaymentServicePlatform provider = it->first;
        if (provider != MyGame)
        {
            ShopLibrary& table = g_ShopStore[provider].table;
            ShopLibrary::iterator catIt = table.find(propertyId);
            if (catIt != table.end())
            {
                ShopItemTable& itemTable = catIt->second;
                ShopItemTable::iterator itemIt = itemTable.find(gameItemId);
                if (itemIt != itemTable.end())
                {
                    lists.push_back(provider);
                    isSupported = true;
                }
            }
        }
    }
    return isSupported;
}

void SetShopDiscountList(stShopDiscountList& lists)
{
    g_Discount.clear();
    int count = lists.size();
    for (int i = 0; i < count; i++)
    {
        g_Discount.push_back(lists[i]);
    }
}

bool GetShopDiscount(int category, stShopDiscountList& discount)
{
    bool hasResult = false;
    int count = g_Discount.size();
    for (int i = 0; i < count; i++)
    {
        if (g_Discount[i].category == category)
        {
            discount.push_back(g_Discount[i]);
            hasResult = true;
        }
    }
    return hasResult;
}

void SetCurrentPayPoint(const enum ShopPayPointType paypoint)
{
    g_CurrentPayPoint = paypoint;
}

ShopPayPointType GetCurrentPayPoint()
{
    return g_CurrentPayPoint;
}

bool ShopItemPriceCmp(const stShopItemDetailInfo& one, const stShopItemDetailInfo& other)
{
    return one.GetItemPriceValue() < other.GetItemPriceValue();
}

void GetShopItemList(const PaymentServicePlatform provider, const enum ShopPayPointType paypoint, const stPropertyId& propertyId, ShopItemList &lists)
{
    const stProperty &property = g_PropertyLibrary[propertyId];
    ShopLibrary& table = g_ShopStore[provider].table;
    ShopLibrary::iterator catIt = table.find(propertyId);
    if (catIt != table.end())
    {
        ShopItemTable& itemTable = catIt->second;
        for (ShopItemTable::iterator it = itemTable.begin(); it != itemTable.end(); ++it)
        {
            if (it->second.ContainsEntryPoint(paypoint)) {
                stShopItemDetailInfo item = it->second;
                item.MergePropertyEffectAttributes(property);
                lists.push_back(item);
            }
        }
    }
}

void GetSortShopItemList(const PaymentServicePlatform provider, const enum ShopPayPointType paypoint, const stPropertyId& propertyId, ShopItemList &lists)
{
    SetCurrentPayPoint(paypoint);
    
    if (propertyId == PROPERTY_DEFAULT)
    {
        for (PropertyLibrary::iterator it = g_PropertyLibrary.begin(); it != g_PropertyLibrary.end(); ++ it)
        {
            stPropertyId _propertyId = it->first;
            GetShopItemList(provider, paypoint, _propertyId, lists);
        }
    }
    else
    {
        GetShopItemList(provider, paypoint, propertyId, lists);
    }
    std::sort(lists.begin(), lists.end(), ShopItemPriceCmp);
}

bool FindBestChargeItem( BestChargeItemContext& context, stShopItemDetailInfo& info )
{
    stShopItemDetailInfo result;
    int cost = INT_MAX;
    bool bFoundAny = false;
    int quantity = context.chargeQuantity;
    for(ShopStoreIter shopLibIt = g_ShopStore.begin(); shopLibIt != g_ShopStore.end(); ++shopLibIt)
    {
        if (shopLibIt->first == MyGame)
        {
            //remove myGame provider for recharge imple
            continue;
        }
        ShopCache& shopCache = shopLibIt->second;
        for (ShopLibraryIter shopItemTableIt = shopCache.table.begin(); shopItemTableIt != shopCache.table.end(); ++shopItemTableIt)
        {
            ShopItemTable& shopTable = shopItemTableIt->second;
            for (ShopItemTableIter shopItemIt = shopTable.begin(); shopItemIt != shopTable.end(); ++shopItemIt)
            {
                stShopItemDetailInfo item = shopItemIt->second;
                if ((item.ContainsEntryPoint(context.paymentPoints))
                    && (item.ContainsPaymentMethod(context.paymentMethods))
                    && (context.itemPredictor(context, item))
                    )
                {
                    int myCost = context.costEvaluator(context, item);
                    if (myCost < cost)
                    {
                        cost = myCost;
                        result = item;
                        bFoundAny = true;
                        quantity = context.chargeQuantity;
                    }
                }
            }
        }
    }
    if (bFoundAny) {
        context.chargeQuantity = quantity;
        info = result;
    } else {
        context.chargeQuantity = 0;
    }
    return bFoundAny;
}

bool StandardCoinsChargeItemPredictor( BestChargeItemContext& context, const stShopItemDetailInfo& info)
{
    int singleItemChargeValue = info.GetFinalProperyEffectAttribute(PROPERTY_GOLD_COINS);
    if (singleItemChargeValue <= 0)
    {
        return false;
    }
    if (info.AcceptsQuantity())
    {
        context.chargeQuantity = (int)ceilf(float(context.chargeValue) / singleItemChargeValue);
        return true;
    }
    context.chargeQuantity = 1;
    return singleItemChargeValue >= context.chargeValue;
}

bool StandardDiamondsChargeItemPredictor( BestChargeItemContext& context, const stShopItemDetailInfo& info)
{
    int singleItemChargeValue = info.GetFinalProperyEffectAttribute(PROPERTY_DIAMONDS);
    if (singleItemChargeValue <= 0)
    {
        return false;
    }
    if (info.AcceptsQuantity())
    {
        context.chargeQuantity = (int)ceilf(float(context.chargeValue) / singleItemChargeValue);
        return true;
    }
    context.chargeQuantity = 1;
    return singleItemChargeValue >= context.chargeValue;
}

int StandardCostEvaluator( const BestChargeItemContext& context, const stShopItemDetailInfo& info )
{
    float factor = 1.0f;
    switch (info.GetItemPayCurrency())
    {
    case PAYMENT_CURRENCY_MONEY:
        factor = 10000.0f;
        break;
    case PAYMENT_CURRENCY_GOLDS:
        factor = 1.0f;
        break;
    }
    return int(factor * info.GetItemPriceValue() * context.chargeQuantity);
}

BestChargeItemContext::BestChargeItemContext() : paymentPoints(0)
, paymentMethods(0)
, chargeValue(0)
, chargeQuantity(0)
, itemPredictor(NULL)
, costEvaluator(StandardCostEvaluator)
{

}


#endif
