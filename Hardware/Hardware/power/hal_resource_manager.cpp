#include <RMClient.h>
#include <RMClientListener.h>
#include <RMRequest.h>
#include <RMResponse.h>
#include <utils/RefBase.h>

using namespace android;

struct StrRMClientListener : public RMClientListener {
    StrRMClientListener();
    virtual ~StrRMClientListener();

    virtual void handleRequestDone(const sp<RMResponse> &response);
    virtual bool handleResourcePreempted();
    virtual void handleAllResourcesReturned();
    sp<ValueUpdateListener<bool> > mRequestDone;
    sp<ValueUpdateListener<bool> > mResourceSatisfied;
    sp<ValueUpdateListener<bool> > mResourceKeeped;
    sp<RMResponse> mResponse;
};

StrRMClientListener::StrRMClientListener()
:mRequestDone(new ValueUpdateListener<bool>(false))
,mResourceSatisfied(new ValueUpdateListener<bool>(false))
,mResourceKeeped(new ValueUpdateListener<bool>(false))
{
}

StrRMClientListener::~StrRMClientListener() {
}

void StrRMClientListener::handleRequestDone(const sp<RMResponse> &response) {
    ALOGD("StrRMClientListener handleRequestDone()\n");
    mResponse = response;
    mResourceSatisfied->set(true);
    mResourceKeeped->set(true);
    mRequestDone->set(true);
}

bool StrRMClientListener::handleResourcePreempted() {
    return false;
}

void StrRMClientListener::handleAllResourcesReturned() {
    mResourceKeeped->set(false);
}

sp<RMClient> mRMClient;
sp<StrRMClientListener> mRMListener;
static const uint64_t kWaitIntervalUs = 10000000UL;
static bool bStrRMHasInit = false;

void RM_init() {
    mRMListener = new StrRMClientListener();
    ALOGD("Str RMClient\n");
    mRMClient = new RMClient(mRMListener,kRMClientPriorityMax);
    bStrRMHasInit = true;
}

void STR_allocate_all_resource() {
    SLOGD("hal_resource_manager:  STR allocateAllResource() \n");
    mRMListener->mResourceSatisfied->set(false);
    mRMListener->mRequestDone->set(false);

    if (mRMListener->mResourceKeeped->get() == true) {
        mRMClient->returnAllResources();
    }

    sp<RMRequest> req = new RMRequest();
    req->addComponent(kComponentVideoRenderer);
    req->addComponent(kComponentNativeVideoPlaneMain);
    req->addComponent(kComponentNativeVideoPlaneSub);
    req->addCapability(kRMCapabilityHdmi, 1);
    req->addComponent(kComponentVideoDecoder);

    mRMClient->requestResourcesAsync(req);

    mRMListener->mRequestDone->waitForValue(true, kWaitIntervalUs);

    if (mRMListener->mResourceSatisfied->get() == true) {
        SLOGD("STR allocateAllResource() Success\n");
    } else {
        SLOGD("STR allocateAllResource() Fail\n");
    }
}

void STR_release_all_resource() {
    SLOGD("STR releaseAllResource() \n");
    if(!bStrRMHasInit)
    {
        SLOGD("STR RMClient not init, return \n");
        return;
    }
    if (mRMListener->mResourceKeeped->get() == true) {
        SLOGD("STR call returnAllResources() \n");
        mRMClient->returnAllResources();
    }
}
