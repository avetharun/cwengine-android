#include "ani.h"
using namespace ani;
#include "ndk-helper/JNIHelper.h"
#include "include/abt/cwerror.h"
#include "rsm_ui_man.h"
#include <stdlib.h>
#include <linux/usbdevice_fs.h>
#include <linux/usbip.h>
#include <linux/usb/f_accessory.h>
#include <linux/usb/video.h>
#include <sys/ioctl.h>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
using namespace ani;

JNIEnv* env;
JavaVM* vm;
struct android_app* app_g;
jobject clazz;

struct android_app* ANIEnv::app() { return app_g; }

std::map<int, std::string> VersionCodeStrings__ = {
	{Build::VERSION_CODES::CUR_DEVELOPMENT,							"Current Development Build"},
	{Build::VERSION_CODES::BASE,									"Base"},
	{Build::VERSION_CODES::BASE_1_1,								"Base 1.1"},
	{Build::VERSION_CODES::CUPCAKE,									"Cupcake"},
	{Build::VERSION_CODES::DONUT,									"Donut"},
	{Build::VERSION_CODES::ECLAIR,									"Eclair"},
	{Build::VERSION_CODES::ECLAIR_0_1,								"Eclair 0.1 (A2.0.1)"},
	{Build::VERSION_CODES::ECLAIR_MR1,								"Eclair MR1 (A2.1)"},
	{Build::VERSION_CODES::FROYO,									"Froyo"},
	{Build::VERSION_CODES::GINGERBREAD,								"Gingerbread"},
	{Build::VERSION_CODES::GINGERBREAD_MR1,							"Gingerbread MR1 (A2.3.3)"},
	{Build::VERSION_CODES::HONEYCOMB,								"Honeycomb"},
	{Build::VERSION_CODES::HONEYCOMB_MR1,							"Honeycomb MR1 (A3.1)"},
	{Build::VERSION_CODES::HONEYCOMB_MR2,							"Honeycomb MR2 (A3.2)"},
	{Build::VERSION_CODES::ICE_CREAM_SANDWICH,						"Icecream Sandwich"},
	{Build::VERSION_CODES::ICE_CREAM_SANDWICH_MR1,					"Icecream Sandwich MR1 (A4.03)"},
	{Build::VERSION_CODES::JELLY_BEAN,								"Jellybean"},
	{Build::VERSION_CODES::JELLY_BEAN_MR1,							"Jellybean MR1 (A4.2)"},
	{Build::VERSION_CODES::JELLY_BEAN_MR2,							"Jellybean MR2 (A4.3)"},
	{Build::VERSION_CODES::KITKAT,									"KitKat"},
	{Build::VERSION_CODES::KITKAT_WATCH,							"KitKat W (A4.4W)"},
	{Build::VERSION_CODES::LOLLIPOP,								"Lollipop"},
	{Build::VERSION_CODES::LOLLIPOP_MR1,							"Lollipop MR1 (A5.1)"},
	{Build::VERSION_CODES::MARSHMALLOW,								"Marshmallow"},
	{Build::VERSION_CODES::NOUGAT,									"Nougat"},
	{Build::VERSION_CODES::NOUGAT_MR1,								"Nougat MR1 (A7.1)"},
	{Build::VERSION_CODES::OREO,									"Oreo"},
	{Build::VERSION_CODES::OREO_MR1,								"Oreo MR1 (A8.1)"},
	{Build::VERSION_CODES::PIE,										"Pie"},
	{Build::VERSION_CODES::Q,										"Android 10"},
	{Build::VERSION_CODES::R,										"Android 11"},
	{Build::VERSION_CODES::S,										"Android S"},
	{Build::VERSION_CODES::S_V2,									"Android S V2"},
	{Build::VERSION_CODES::TIRAMISU,								"Tiramisu"},
	{Build::VERSION_CODES::WINDOWS_SUBSYSTEM_FOR_ANDROID,           u8"Windows Subsystem for Android\xE2\x84\xA2"}
};
const char* __rootusb_class = "android.hardware.usb";
const char* rootcls = "android.content.Context";

JavaVMAttachArgs args{
version : JNI_VERSION_1_1,
name: "ANI Thread"
};
static jclass __clazz_syssz = 0;
jclass ani::SystemService::GetClass() {
	return __clazz_syssz = ANIEnv::EnsureEnv()->FindClass(rootcls);
}
struct Networking::HeaderType {

	static inline const std::list<std::string> JSON = {
		"Content-Type: application/json"
	};
	static inline const std::list<std::string> PlainText = {
		"Content-Type: text/plain"
	};
};
static std::string __curl_request_data = "";
static CURLcode last_curl_response = {};
static bool _is_curl_init__ = false;
size_t __curl_WriteStringCallback(char* ptr, size_t size, size_t nmemb)
{
	__curl_request_data += std::string(ptr, size * nmemb);
	int totalSize = size * nmemb;
	return totalSize;
}
// Returns a pointer to a new JNIEnv if it's not already initialized.
void ANIEnv::EnsureEnvV(JavaVM* thisvm, JNIEnv* _env) {
	int gstat = thisvm->GetEnv((void**) & env, JNI_VERSION_1_6);
	if (gstat == JNI_EDETACHED) {
		if (gstat = thisvm->AttachCurrentThread(&env, NULL) != 0) {
			LOGF("Unable to attach to VM, GetEnv and AttachCurrentThread failed.");
			exit(-1);
		};
	}
	if (gstat == JNI_EVERSION) {
		LOGF("Wrong version, JNI_VERSION_1_6 unsupported by this device.");
	}
	LOGI("ANIEnv successfully attached to VM");
}

// Returns a pointer to a new JNIEnv if it's not already initialized.
JNIEnv* ANIEnv::EnsureEnv(JavaVM* vm) {
	EnsureEnvV(vm, (JNIEnv*)env);
	return env;
}
JNIEnv* ANIEnv::EnsureEnv() {
	return EnsureEnv(ANIEnv::getVM());
}

int ANIEnv::init(JavaVM* __vm, const char* app_namespace, jobject AndroidAppClazz, android_app* app) {
	int state_ = 0;
	clazz = AndroidAppClazz;
	vm = __vm;
	ANIEnv::EnsureEnv(__vm);
	app_g = app;
	return 1;
}
#define R_ID_CONTENT 0x01020002
jobject ANIEnv::getRootView() { 
	ANIEnv::EnsureEnv();
	return env->CallObjectMethod(ANIEnv::app()->activity->clazz, env->GetMethodID(env->GetObjectClass(ANIEnv::app()->activity->clazz), "requireViewById", "(I)Landroid/view/View;"), R_ID_CONTENT);
}
jobject ANIEnv::appClass() { return clazz; }
JavaVM* ANIEnv::getVM() { return vm; }
ani::SharedPreferences_Editor::SharedPreferences_Editor(JNIEnv* env, const jobject joSharedPreferences_Edit) :env(env), joSharedPreferences_Edit(joSharedPreferences_Edit) {
	//find the methods for putting values into Shared preferences via the editor
	jclass jcSharedPreferences_Editor = env->GetObjectClass(joSharedPreferences_Edit);
	jmPutBoolean = env->GetMethodID(jcSharedPreferences_Editor, "putBoolean", "(Ljava/lang/String;Z)Landroid/content/SharedPreferences$Editor;");
	jmPutInt = env->GetMethodID(jcSharedPreferences_Editor, "putInt", "(Ljava/lang/String;I)Landroid/content/SharedPreferences$Editor;");
	jmPutString = env->GetMethodID(jcSharedPreferences_Editor, "putString", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;");
	jmPutFloat = env->GetMethodID(jcSharedPreferences_Editor, "putFloat", "(Ljava/lang/String;F)Landroid/content/SharedPreferences$Editor;");
	jmPutLong = env->GetMethodID(jcSharedPreferences_Editor, "putLong", "(Ljava/lang/String;J)Landroid/content/SharedPreferences$Editor;");
	jmClear = env->GetMethodID(jcSharedPreferences_Editor, "clear", "()Landroid/content/SharedPreferences$Editor;");
	jmCommit = env->GetMethodID(jcSharedPreferences_Editor, "commit", "()Z");
	jmApply = env->GetMethodID(jcSharedPreferences_Editor, "apply", "()V");
	jmRemove = env->GetMethodID(jcSharedPreferences_Editor, "remove", "(Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;");
}

//return itself for method chaining

const SharedPreferences_Editor& ani::SharedPreferences_Editor::putBoolean(const char* key, const bool value) const {
	env->CallObjectMethod(joSharedPreferences_Edit, jmPutBoolean, env->NewStringUTF(key), (jboolean)value);
	return *this;
}

const SharedPreferences_Editor& ani::SharedPreferences_Editor::putInt(const char* key, const int value) const {
	env->CallObjectMethod(joSharedPreferences_Edit, jmPutInt, env->NewStringUTF(key), (jint)value);
	return *this;
}

const SharedPreferences_Editor& ani::SharedPreferences_Editor::putString(const char* key, std::string value) const {
	env->CallObjectMethod(joSharedPreferences_Edit, jmPutString, env->NewStringUTF(key), env->NewStringUTF(value.c_str()));
	return *this;
}

const SharedPreferences_Editor& ani::SharedPreferences_Editor::putLong(const char* key, float value) const {
	env->CallObjectMethod(joSharedPreferences_Edit, jmPutLong, env->NewStringUTF(key), (jlong)value);
	return *this;
}

const SharedPreferences_Editor& ani::SharedPreferences_Editor::putFloat(const char* key, float value) const {
	env->CallObjectMethod(joSharedPreferences_Edit, jmPutFloat, env->NewStringUTF(key), (jfloat)value);
	return *this;
}

bool ani::SharedPreferences_Editor::commit() const {
	return (bool)env->CallBooleanMethod(joSharedPreferences_Edit, jmCommit);
}

void ani::SharedPreferences_Editor::apply() const {
	env->CallVoidMethod(joSharedPreferences_Edit, jmApply);
}

const SharedPreferences_Editor& ani::SharedPreferences_Editor::clear() const {
	env->CallObjectMethod(joSharedPreferences_Edit, jmClear);
	return *this;
}

const SharedPreferences_Editor& ani::SharedPreferences_Editor::remove(const char* key) const {
	env->CallObjectMethod(joSharedPreferences_Edit, jmRemove, env->NewStringUTF(key));
	return *this;
}

//Note: Per default, this doesn't keep the reference to the sharedPreferences java object alive
//longer than the lifetime of the JNIEnv.
//With keepReference=true the joSharedPreferences is kept 'alive' and you can still use the class after the original JNIEnv* has become invalid -
//but make sure to refresh the JNIEnv* object with a new valid reference via replaceJNI()

ani::SharedPreferences::SharedPreferences(JNIEnv* _env, jobject activity_clazz, const char* name, const bool keepReference) {
	ani::SharedPreferences::env = _env;
	//Find the 2 java classes we need to make calls with
	jclass jcContext = env->FindClass("android/content/Context");
	jclass jcSharedPreferences = env->FindClass("android/content/SharedPreferences");
	//jclass jcSharedPreferences_Editor=env->FindClass("android/content/SharedPreferences$Editor");
	if (jcContext == nullptr || jcSharedPreferences == nullptr) {
		__android_log_print(ANDROID_LOG_DEBUG, "SharedPreferences", "Cannot find classes");
	}
	//find the 3 functions we need to get values from an SharedPreferences instance
	jmGetBoolean = env->GetMethodID(jcSharedPreferences, "getBoolean", "(Ljava/lang/String;Z)Z");
	jmGetInt = env->GetMethodID(jcSharedPreferences, "getInt", "(Ljava/lang/String;I)I");
	jmGetLong = env->GetMethodID(jcSharedPreferences, "getLong", "(Ljava/lang/String;J)J");
	jmGetFloat = env->GetMethodID(jcSharedPreferences, "getFloat", "(Ljava/lang/String;F)F");
	jmGetString = env->GetMethodID(jcSharedPreferences, "getString", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
	//find the 1 function we need to create the SharedPreferences.Editor object
	jmEdit = env->GetMethodID(jcSharedPreferences, "edit", "()Landroid/content/SharedPreferences$Editor;");
	//create a instance of SharedPreferences and store it in @joSharedPreferences
	jmethodID jmGetSharedPreferences = env->GetMethodID(jcContext, "getSharedPreferences", "(Ljava/lang/String;I)Landroid/content/SharedPreferences;");
	joSharedPreferences = env->CallObjectMethod(activity_clazz, jmGetSharedPreferences, env->NewStringUTF(name), MODE_PRIVATE);
	//jmEdit_commit=env->GetMethodID(jcSharedPreferences_Editor,"putString","(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;");
	jmContains = env->GetMethodID(jcSharedPreferences, "contains", "(Ljava/lang/String;)Z");
	if (keepReference) {
		joSharedPreferences = env->NewWeakGlobalRef(joSharedPreferences);
	}
}

void ani::SharedPreferences::replaceJNI(JNIEnv* newEnv) {
	env = newEnv;
}

bool ani::SharedPreferences::getBoolean(const char* id, bool defaultValue) const {
	return (bool)(env->CallBooleanMethod(joSharedPreferences, jmGetBoolean, env->NewStringUTF(id), (jboolean)defaultValue));
}

int ani::SharedPreferences::getInt(const char* id, int defaultValue) const {
	return (int)(env->CallIntMethod(joSharedPreferences, jmGetInt, env->NewStringUTF(id), (jint)defaultValue));
}

float ani::SharedPreferences::getFloat(const char* id, float defaultValue) const {
	return (float)(env->CallFloatMethod(joSharedPreferences, jmGetFloat, env->NewStringUTF(id), (jfloat)defaultValue));
}

long ani::SharedPreferences::getLong(const char* id, long defaultValue) const {
	return (long)(env->CallFloatMethod(joSharedPreferences, jmGetLong, env->NewStringUTF(id), (jlong)defaultValue));
}

std::string ani::SharedPreferences::getString(const char* id, const char* defaultValue) const {
	auto value = (jstring)(env->CallObjectMethod(joSharedPreferences, jmGetString, env->NewStringUTF(id), env->NewStringUTF(defaultValue)));
	const char* valueP = env->GetStringUTFChars(value, nullptr);
	const std::string ret = std::string(valueP);
	env->ReleaseStringUTFChars(value, valueP);
	//__android_log_print(ANDROID_LOG_DEBUG, "SharedPreferences","%s",ret.c_str());
	return ret;
}

void ani::SharedPreferences::getBoolean(const char* id, bool* out, bool defaultValue) const {
	*out = (bool)(env->CallBooleanMethod(joSharedPreferences, jmGetBoolean, env->NewStringUTF(id), (jboolean)defaultValue));
}

void ani::SharedPreferences::getInt(const char* id, int* out, int defaultValue) const {
	*out = (int)(env->CallIntMethod(joSharedPreferences, jmGetInt, env->NewStringUTF(id), (jint)defaultValue));
}

void ani::SharedPreferences::getFloat(const char* id, float* out, float defaultValue) const {
	*out = (float)(env->CallFloatMethod(joSharedPreferences, jmGetFloat, env->NewStringUTF(id), (jfloat)defaultValue));
}

void ani::SharedPreferences::getLong(const char* id, long* out, long defaultValue) const {
	*out = (long)(env->CallFloatMethod(joSharedPreferences, jmGetLong, env->NewStringUTF(id), (jlong)defaultValue));
}

void ani::SharedPreferences::getString(const char* id, std::string* out, const char* defaultValue) const {
	auto value = (jstring)(env->CallObjectMethod(joSharedPreferences, jmGetString, env->NewStringUTF(id), env->NewStringUTF(defaultValue)));
	const char* valueP = env->GetStringUTFChars(value, nullptr);
	const std::string ret = std::string(valueP);
	env->ReleaseStringUTFChars(value, valueP);
	//__android_log_print(ANDROID_LOG_DEBUG, "SharedPreferences","%s",ret.c_str());
	*out = ret;
}

void ani::SharedPreferences::getString(const char* id, char* out, size_t out_len, const char* defaultValue) const {
	auto value = (jstring)(env->CallObjectMethod(joSharedPreferences, jmGetString, env->NewStringUTF(id), env->NewStringUTF(defaultValue)));
	const char* valueP = env->GetStringUTFChars(value, nullptr);
	const std::string ret = std::string(valueP);
	env->ReleaseStringUTFChars(value, valueP);
	//__android_log_print(ANDROID_LOG_DEBUG, "SharedPreferences","%s",ret.c_str());
	strncpy(out, ret.c_str(), out_len);
}

bool ani::SharedPreferences::contains(std::string id) {
	return env->CallBooleanMethod(joSharedPreferences, jmContains, env->NewStringUTF(id.c_str()));
}

SharedPreferences_Editor ani::SharedPreferences::edit() const {
	//create a instance of SharedPreferences.Editor and store it in @joSharedPreferences_Edit
	jobject joSharedPreferences_Edit = env->CallObjectMethod(joSharedPreferences, jmEdit);
	SharedPreferences_Editor editor(env, joSharedPreferences_Edit);
	return editor;
}

void ani::Networking::openURLBrowser(const char* url) {
	JNIEnv* env = ANIEnv::EnsureEnv();
	jstring url_string = env->NewStringUTF(url);
	jclass uri_class = env->FindClass("android/net/Uri");
	jmethodID uri_parse = env->GetStaticMethodID(uri_class, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");
	jobject uri = env->CallStaticObjectMethod(uri_class, uri_parse, url_string);
	jclass intent_class = env->FindClass("android/content/Intent");
	jfieldID action_view_id = env->GetStaticFieldID(intent_class, "ACTION_VIEW", "Ljava/lang/String;");
	jobject action_view = env->GetStaticObjectField(intent_class, action_view_id);
	jmethodID new_intent = env->GetMethodID(intent_class, "<init>", "(Ljava/lang/String;Landroid/net/Uri;)V");
	jobject intent = env->AllocObject(intent_class);
	env->CallVoidMethod(intent, new_intent, action_view, uri);
	jclass activity_class = env->FindClass("android/app/Activity");
	jmethodID start_activity = env->GetMethodID(activity_class, "startActivity", "(Landroid/content/Intent;)V");
	env->CallVoidMethod(clazz, start_activity, intent);
}

CURL* ani::Networking::impl_curl_init() {
	if (!_is_curl_init__) {
		std::atexit(curl_global_cleanup);
		curl_global_init(CURL_GLOBAL_ALL);
		_is_curl_init__ = true;
	}
	return curl_easy_init();
}

void ani::Networking::ImplRequestInit() {
	__curl_request_data.clear();
}

void ani::Networking::sendPostRequestDataImpl__(const char* url, std::string body, std::string* response, std::list<std::string> headers) {
	response->clear();
	curlpp::Cleanup curlCleanup;
	try {
		curlpp::Easy curlRequest;
		curlRequest.reset();
		curlpp::options::WriteFunction wf(__curl_WriteStringCallback);
		curlRequest.setOpt(wf);
		curlRequest.setOpt(new curlpp::options::HttpHeader(headers));
		curlRequest.setOpt(curlpp::options::Url(url));
		curlRequest.setOpt(new curlpp::options::PostFields(body));
		curlRequest.setOpt(new curlpp::options::PostFieldSize(body.length()));
		curlRequest.perform();
		*response = __curl_request_data;
	}
	catch (curlpp::LibcurlRuntimeError e) {
		*response = std::string(e.what()) + "| Curl returned : " + curl_easy_strerror(e.whatCode());
	}
	catch (curlpp::LogicError e) {
		*response = std::string(e.what());
	}
	ImplRequestInit();
}

void ani::Networking::ImplGetRequestHandler__(const char* url, std::string* response) {
	response->clear();
	curlpp::Cleanup curlCleanup;
	try {
		curlpp::Easy curlRequest;
		curlRequest.reset();
		curlpp::options::WriteFunction wf(__curl_WriteStringCallback);
		curlRequest.setOpt(wf);
		curlRequest.setOpt(curlpp::options::Url(url));
		curlRequest.perform();
		*response = __curl_request_data;
	}
	catch (curlpp::LibcurlRuntimeError e) {
		*response = std::string(e.what()) + "| Curl returned : " + curl_easy_strerror(e.whatCode());
	}
	catch (curlpp::LogicError e) {
		*response = std::string(e.what());
	}
	ImplRequestInit();
}

// Assumes <response> will be valid upon request finished. Ensure it doesn't get deleted using shared_ptr or a static variable!
// *response will automatically be cleared before the request is sent.

void ani::Networking::sendGetRequestAsync(const char* url, std::string* response) {
	response->clear();
	std::thread t(ImplGetRequestHandler__, url, response);
	t.detach();
}

std::string ani::Networking::sendGetRequest(const char* url) {
	std::string out = "";
	ImplGetRequestHandler__(url, &out);
	return out;
}

// Assumes <response> will be valid upon request finished. Ensure it doesn't get deleted using shared_ptr or a static variable!
// *response will automatically be cleared before the request is sent.

void ani::Networking::sendPostRequestAsync(const char* url, std::string body, std::string* response, std::list<std::string> headers) {
	response->clear();
	std::thread t(sendPostRequestDataImpl__, url, body, response, headers);
	t.detach();
}

std::string ani::Networking::sendPostRequest(const char* url, std::string body, std::list<std::string> headers) {
	std::string out = "";
	sendPostRequestDataImpl__(url, body, &out, headers);
	return out;
}


jobject ani::SystemService::getSystemService(const char* service_type) {
	jmethodID __id = ANIEnv::EnsureEnv()->GetMethodID(GetClass(), "getSystemService", "(Ljava/lang/String;)Ljava/lang/String;");
	jstring __s_arg = ANIEnv::EnsureEnv()->NewStringUTF(service_type);
	jobject __sysobj = ANIEnv::EnsureEnv()->CallStaticObjectMethod(__clazz_syssz, __id, __s_arg);
	ANIEnv::EnsureEnv()->ReleaseStringUTFChars(__s_arg, service_type);
	return __sysobj;
}

jobject ani::getActivityContext(JNIEnv* env, jobject activity_object) {
	//jclass _aclzz = env->GetObjectClass(activity_object);
	//jmethodID _gctxf = env->GetMethodID(_aclzz, "getContext", "()Landroid/content/Context;");
	//jobject context = env->CallObjectMethod(activity_object, _gctxf);
	return activity_object;
}

jobject ani::getGlobalContext(JNIEnv* env)
{
	jclass activityThread = env->FindClass("android/app/ActivityThread");
	jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
	jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);

	jmethodID getApplication = env->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");
	jobject context = env->CallObjectMethod(at, getApplication);
	return context;
}

float ani::get_refresh_rate() {
	JNIEnv* env = ANIEnv::EnsureEnv();
	jobject _ctxo = getActivityContext(env, ANIEnv::app()->activity->clazz);
	jclass _ctxc = env->GetObjectClass(_ctxo);
	jmethodID _gdsp_o = env->GetMethodID(_ctxc, "getDisplay", "()Landroid/view/Display;");
	jobject _dsp_o = env->CallObjectMethod(_ctxo, _gdsp_o);
	jclass _dsp_o_c = env->GetObjectClass(_dsp_o);
	jmethodID _dsp_rate = env->GetMethodID(_dsp_o_c, "getRefreshRate", "()F");
	jfloat _out = env->CallFloatMethod(_dsp_o, _dsp_rate);
	return _out;
}

bool ani::adb_enabled() {
	bool out = false;
	JNIEnv* jni;
	ANIEnv::app()->activity->vm->AttachCurrentThread(&jni, NULL);
	jclass c_settings_secure = jni->FindClass("android/provider/Settings$Secure");
	jclass c_context = jni->FindClass("android/content/Context");
	//Get the getContentResolver method
	jmethodID m_get_content_resolver = jni->GetMethodID(c_context, "getContentResolver",
		"()Landroid/content/ContentResolver;");
	//Get the Settings.Secure.ANDROID_ID constant
	jfieldID f_android_id = jni->GetStaticFieldID(c_settings_secure, "ADB_ENABLED", "Ljava/lang/String;");
	jstring s_android_id = (jstring)jni->GetStaticObjectField(c_settings_secure, f_android_id);
	//create a ContentResolver instance context.getContentResolver()
	jobject o_content_resolver = jni->CallObjectMethod(getGlobalContext(jni), m_get_content_resolver);
	jmethodID m_get_int = jni->GetStaticMethodID(c_settings_secure, "getInt",
		"(Landroid/content/ContentResolver;Ljava/lang/String;)I");
	//get the setting value
	jint android_id = jni->CallStaticIntMethod(c_settings_secure,
		m_get_int,
		o_content_resolver,
		s_android_id);
	if (android_id == 1) { out = true; }
	ANIEnv::app()->activity->vm->DetachCurrentThread();
	return out;
}
float ani::getDPI() {

	JNIEnv* jni;
	ANIEnv::app()->activity->vm->AttachCurrentThread(&jni, NULL);
	bool state = false; jclass cls = jni->GetObjectClass(ANIEnv::app()->activity->clazz);
	jmethodID getResourcesMID = jni->GetMethodID(cls, "getResources", "()Landroid/content/res/Resources;");
	jobject resources = jni->CallObjectMethod(ANIEnv::app()->activity->clazz, getResourcesMID);
	jclass resources_cls = jni->GetObjectClass(resources);
	jmethodID getDisplayMetricsID = jni->GetMethodID(resources_cls, "getDisplayMetrics", "()Landroid/util/DisplayMetrics;");
	jobject dMet = jni->CallObjectMethod(resources, getDisplayMetricsID);
	jclass dMetCls = jni->GetObjectClass(dMet);

	jfieldID densityDpi = jni->GetFieldID(dMetCls, "xdpi", "F");
	jfloat dpi = jni->GetFloatField(dMet, densityDpi);

	ANIEnv::app()->activity->vm->DetachCurrentThread();
	return dpi;
}
int ani::AndroidGenericDimenShown(const char* bar_name, int* height) {
	JNIEnv* jni;
	ANIEnv::app()->activity->vm->AttachCurrentThread(&jni, NULL);
	bool state = false; jclass cls = jni->GetObjectClass(ANIEnv::app()->activity->clazz);
	jmethodID getResourcesMID = jni->GetMethodID(cls, "getResources", "()Landroid/content/res/Resources;");
	jobject resources = jni->CallObjectMethod(ANIEnv::app()->activity->clazz, getResourcesMID);
	jclass resources_cls = jni->GetObjectClass(resources);
	jmethodID getIDMID = jni->GetMethodID(resources_cls, "getIdentifier", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
	jmethodID getDimensionPixelSizeMID = jni->GetMethodID(resources_cls, "getDimensionPixelSize", "(I)I");
	jstring id0 = jni->NewStringUTF(bar_name);
	jstring id1 = jni->NewStringUTF("dimen");
	jstring id2 = jni->NewStringUTF("android");
	/*get the resource id for the status bar */
	jint resid = jni->CallIntMethod(resources, getIDMID, id0, id1, id2);
	int pxheight = 0;
	if (resid > 0) {
		pxheight = jni->CallIntMethod(resources, getDimensionPixelSizeMID, resid);
		/*get status bar height*/
	}
	if (height != NULL) {
		*height = pxheight;
	}
	jni->DeleteLocalRef(id0);
	jni->DeleteLocalRef(id1);
	jni->DeleteLocalRef(id2);
	ANIEnv::app()->activity->vm->DetachCurrentThread();
	return pxheight > 0;
}

int ani::AndroidStatusBarShown(int* height) {
	return AndroidGenericDimenShown("status_bar_height", height);
}

int ani::AndroidStatusBarHeight() {
	int out;
	AndroidStatusBarShown(&out);
	return out;
}

int ani::AndroidNavigationBarShown(int* height) {
	return AndroidGenericDimenShown("navigation_bar_height", height);
}

int ani::AndroidNavigationBarHeight() {
	int out;
	AndroidNavigationBarShown(&out);
	return out;
}

int ani::AndroidKeyboardShown(bool return_height) {
	JNIEnv* jni;
	ANIEnv::app()->activity->vm->AttachCurrentThread(&jni, NULL);
	bool state = false;
	jclass cls = jni->GetObjectClass(ANIEnv::app()->activity->clazz);
	jmethodID methodID = jni->GetMethodID(cls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	jstring service_name = jni->NewStringUTF("input_method");
	jobject input_service = jni->CallObjectMethod(ANIEnv::app()->activity->clazz, methodID, service_name);
	jclass input_service_cls = jni->GetObjectClass(input_service);
	methodID = jni->GetMethodID(input_service_cls, "getInputMethodWindowVisibleHeight", "()I");
	GlobalState::KeyboardHeight = jni->CallIntMethod(input_service, methodID);
	if (GlobalState::KeyboardHeight > 0) {
		GlobalState::keyboard_visible = true;
		state = true;
	}
	else {
		GlobalState::keyboard_visible = false;
	}
	ANIEnv::app()->activity->vm->DetachCurrentThread();
	if (!return_height) {
		return state;
	}
	else { return GlobalState::KeyboardHeight; }
}

int ani::AndroidKeyboardHeight() {
	return AndroidKeyboardShown(/*return_height = */true);
}

void ani::AndroidToggleKeyboard()
{
	JNIEnv* jni;
	ANIEnv::app()->activity->vm->AttachCurrentThread(&jni, NULL);

	jclass cls = jni->GetObjectClass(ANIEnv::app()->activity->clazz);
	jmethodID methodID = jni->GetMethodID(cls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	jstring service_name = jni->NewStringUTF("input_method");
	jobject input_service = jni->CallObjectMethod(ANIEnv::app()->activity->clazz, methodID, service_name);

	jclass input_service_cls = jni->GetObjectClass(input_service);
	methodID = jni->GetMethodID(input_service_cls, "toggleSoftInput", "(II)V");
	jni->CallVoidMethod(input_service, methodID, 0, 0);

	jni->DeleteLocalRef(service_name);
	ANIEnv::app()->activity->vm->DetachCurrentThread();
}

int ani::GetUnicodeChar(android_app* app, int eventType, int keyCode, int metaState)
{
	JavaVM* javaVM = app->activity->vm;
	JNIEnv* jniEnv = app->activity->env;

	JavaVMAttachArgs attachArgs;
	attachArgs.version = JNI_VERSION_1_6;
	attachArgs.name = "NativeThread";
	attachArgs.group = NULL;

	jint result = javaVM->AttachCurrentThread(&jniEnv, &attachArgs);
	if (result == JNI_ERR)
	{
		return 0;
	}

	jclass class_key_event = jniEnv->FindClass("android/view/KeyEvent");
	int unicodeKey;

	if (metaState == 0)
	{
		jmethodID method_get_unicode_char = jniEnv->GetMethodID(class_key_event, "getUnicodeChar", "()I");
		jmethodID eventConstructor = jniEnv->GetMethodID(class_key_event, "<init>", "(II)V");
		jobject eventObj = jniEnv->NewObject(class_key_event, eventConstructor, eventType, keyCode);

		unicodeKey = jniEnv->CallIntMethod(eventObj, method_get_unicode_char);
	}

	else
	{
		jmethodID method_get_unicode_char = jniEnv->GetMethodID(class_key_event, "getUnicodeChar", "(I)I");
		jmethodID eventConstructor = jniEnv->GetMethodID(class_key_event, "<init>", "(II)V");
		jobject eventObj = jniEnv->NewObject(class_key_event, eventConstructor, eventType, keyCode);

		unicodeKey = jniEnv->CallIntMethod(eventObj, method_get_unicode_char, metaState);
	}

	javaVM->DetachCurrentThread();

	LOGI("Unicode key is: %d", unicodeKey);
	return unicodeKey;
}

int16_t ani::AndroidGetUnicodeChar(int keyCode, int metaState)
{
	//https://stackoverflow.com/questions/21124051/receive-complete-android-unicode-input-in-c-c/43871301

	int eventType = AKEY_EVENT_ACTION_DOWN;
	JNIEnv* jni;
	ANIEnv::app()->activity->vm->AttachCurrentThread(&jni, NULL);

	jclass class_key_event = jni->FindClass("android/view/KeyEvent");
	jclass class_Character = jni->FindClass("java/lang/Character");
	jmethodID method_get_unicode_char = jni->GetMethodID(class_key_event, "getUnicodeChar", "(I)I");
	jmethodID eventConstructor = jni->GetMethodID(class_key_event, "<init>", "(II)V");
	jobject eventObj = jni->NewObject(class_key_event, eventConstructor, eventType, keyCode);

	int unicodeKey = jni->CallIntMethod(eventObj, method_get_unicode_char, metaState);
	ANIEnv::app()->activity->vm->DetachCurrentThread();
	/*int eventType = AKEY_EVENT_ACTION_DOWN;
	JNIEnv* jni;
	ANIEnv::app()->activity->vm->AttachCurrentThread(&jni, NULL);

	jclass class_key_map = jni->FindClass("android/view/KeyCharacterMap");
	jclass class_key_event = jni->FindClass("android/view/KeyEvent");
	jmethodID method_get_id = jni->GetMethodID(class_key_event, "getDeviceId", "()I");
	jmethodID eventConstructor = jni->GetMethodID(class_key_event, "<init>", "(II)V");
	jmethodID mapConstructor = jni->GetStaticMethodID(class_key_map, "load", "(I)Landroid/view/KeyCharacterMap;");
	jmethodID method_get_unicode_char = jni->GetMethodID(class_key_map, "get", "(II)I");

	jobject eventObj = jni->NewObject(class_key_event, eventConstructor, eventType, keyCode);

	jint devid = jni->CallIntMethod(eventObj, method_get_id);
	jobject charmap = jni->CallStaticObjectMethod(class_key_map, mapConstructor, devid);
	uint32_t unicodeKey = jni->CallIntMethod(charmap, method_get_unicode_char, keyCode, metaState);*/
	cwError::serrof("Unicode character ID: %d", unicodeKey);

	//int unicodeKey = jni->CallIntMethod(eventObj, method_get_unicode_char, keyCode, metaState);

	ANIEnv::app()->activity->vm->DetachCurrentThread();


	return unicodeKey;
}

void ani::AndroidShowKeyboard() {
	if (AndroidKeyboardShown()) { return; }
	// Keyboard hidden, enable it.
	AndroidToggleKeyboard();
}

void ani::AndroidHideKeyboard() {
	if (!AndroidKeyboardShown()) { return; }
	// Keyboard shown, disable it
	AndroidToggleKeyboard();
}

void ani::AndroidSetWakeLockState(int state) {
	if (state) {
		// add flag
		ANativeActivity_setWindowFlags(ANIEnv::app()->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);
	}
	if (!state) {
		// remove flag
		ANativeActivity_setWindowFlags(ANIEnv::app()->activity, 0, AWINDOW_FLAG_KEEP_SCREEN_ON);
	}
}
std::unordered_map<std::string, std::string> SysData;
std::string getSysDataValue(std::string path) {
	if (SysData.find(path) == SysData.end()) {
		if (!alib_file_exists(path.c_str())) {
			return "null";
		}
		SysData.insert_or_assign(path, alib_file_read(path.c_str()));
	}
	return SysData.at(path);
}
bool ani::isUsingWSA() {
	return alib_costr(getSysDataValue("/proc/version"), "windows-subsystem-for-android");
}

int ani::Build::SDK_INT() {
	JNIEnv* env = ANIEnv::EnsureEnv();
	jclass _build__version_cls = env->FindClass("android/os/Build$VERSION");
	jint _sdk_int = env->GetStaticIntField(_build__version_cls, env->GetStaticFieldID(_build__version_cls, "SDK_INT", "I"));
	return _sdk_int;
}

std::string ani::Build::GetBuildName(int BuildID) {
	if (VersionCodeStrings__.count(BuildID)) {
		return VersionCodeStrings__[BuildID] + (ani::isUsingWSA() ? "" : " ("+VersionCodeStrings__[ani::Build::VERSION_CODES::WINDOWS_SUBSYSTEM_FOR_ANDROID] + ")");
	}
	else { return "Unknown"; }
}

void ani::Vibrator::Vibrate(float millis) {

}
constexpr auto MEDIA_PLAYER_STATE_PAN_LEFT = 0;
constexpr auto MEDIA_PLAYER_STATE_PAN_RIGHT = 4L;
constexpr auto MEDIA_PLAYER_STATE_PREPARED = 8L;
// Never use this directly.
//jclass _mediaPlayerCls;
jclass getMediaPlayerClass() {
	return ANIEnv::EnsureEnv()->FindClass("android/media/MediaPlayer");
}
bool isPrepared(char* state) {
	return *((bool*)state + MEDIA_PLAYER_STATE_PREPARED);
}
ani::MediaPlayer::MediaPlayer() {
	jclass mdC = getMediaPlayerClass();
	auto ctor = env->GetMethodID(mdC, "<init>", "()V");
	__thiz = std::make_shared<jobject>(env->NewGlobalRef(env->NewObject(mdC, ctor)));
}
void ani::MediaPlayer::operator~() {
	ANIEnv::EnsureEnv()->DeleteGlobalRef(*__thiz);
	__thiz.reset();
}
ani::MediaPlayer::MediaPlayer(std::string fname, bool isFromAPK) : ani::MediaPlayer::MediaPlayer() {
	setDataSource(fname, isFromAPK);
}
void ani::MediaPlayer::setDataSource(std::string fname, bool isFromAPK) {
	*(_ + MEDIA_PLAYER_STATE_PREPARED) = false;
	jclass mdC = getMediaPlayerClass();
	auto APKsDS = env->GetMethodID(mdC, "setDataSource", "(Landroid/content/res/AssetFileDescriptor;)V");
	auto FSsDS = env->GetMethodID(mdC, "setDataSource", "(Ljava/lang/String;)V");
	if (isFromAPK) {
		AAsset* aF = AAssetManager_open(ANIEnv::app()->activity->assetManager, fname.c_str(), AASSET_MODE_BUFFER);
		off64_t begin, len;
		int nFD = AAsset_openFileDescriptor64(aF, &begin, &len);
		jclass paCls = env->FindClass("android/os/ParcelFileDescriptor");
		auto adoFD = env->GetStaticMethodID(paCls, "adoptFd", "(I)Landroid/os/ParcelFileDescriptor;");
		jobject pFD = env->CallStaticObjectMethod(paCls, adoFD, nFD);
		jclass aFDCls = env->FindClass("android/content/res/AssetFileDescriptor");
		jobject jFD = env->NewObject(aFDCls, env->GetMethodID(aFDCls, "<init>", "(Landroid/os/ParcelFileDescriptor;JJ)V"), pFD, (long)begin,(long)len);
		env->CallVoidMethod(*__thiz, APKsDS, jFD);
		return;
	}
	env->CallVoidMethod(*__thiz, FSsDS, env->NewStringUTF(fname.c_str()));
}
bool ani::MediaPlayer::isPlaying() {
	jclass mdC = getMediaPlayerClass();
	ANIEnv::EnsureEnv();
	return env->CallBooleanMethod(*__thiz, env->GetMethodID(mdC, "isPlaying", "()Z"));
}
void ani::MediaPlayer::play() {
	start();
}
void ani::MediaPlayer::stop() {
	if (!isPrepared(_)) {
		LOGW("MediaPlayer hasn't been prepared yet! Automatically preparing..");
		prepare();
		return;
	}
	jclass mdC = getMediaPlayerClass();
	env->CallVoidMethod(*__thiz, env->GetMethodID(mdC, "stop", "()V"));
}
void ani::MediaPlayer::start() {
	if (!isPrepared(_)) {
		LOGW("MediaPlayer hasn't been prepared yet! Automatically preparing..");
		prepare();
	}
	if (isPlaying()) {
		LOGW("MediaPlayer is already playing. Did you mean to call restart()?");
	}
	jclass mdC = getMediaPlayerClass();
	env->CallVoidMethod(*__thiz, env->GetMethodID(mdC, "start", "()V"));
}
void ani::MediaPlayer::restart() {
	seek(0);
	start();
}
void ani::MediaPlayer::pause() {
	jclass mdC = getMediaPlayerClass();
	auto plyM = env->GetMethodID(mdC, "pause", "()V");
	env->CallVoidMethod(*__thiz, plyM);
}
void ani::MediaPlayer::seek(int msec) {
	jclass mdC = getMediaPlayerClass();
	auto skM = env->GetMethodID(mdC, "seekTo", "(I)V");
	env->CallVoidMethod(*__thiz, skM, msec);
}
void ani::MediaPlayer::seekSeconds(double seconds) {
	seek(floor(seconds * 1000.));
}
void ani::MediaPlayer::seekNanos(long long nanos) {
	if (nanos == 0) { seek(0); return; }
	seek(nanos / 1e-6);
}
void ani::MediaPlayer::seekMicros(long long micros) {
	if (micros == 0) { seek(0); return; }
	seek(micros / 1000);
}
void setVolumeState(float left, float right, char* states) {
	*states = left;
	*(states + sizeof(float)) = right;
}
void ani::MediaPlayer::pan(float left, float right) {
	jclass mdC = getMediaPlayerClass();
	setVolumeState(left, right, _);
	auto skM = env->GetMethodID(mdC, "setVolume", "(FF)V");
	env->CallVoidMethod(*__thiz, skM, left, right);
}
ani::MediaPlayer::pan_state_t ani::MediaPlayer::get_pan() {
	return { *(float*)_ + MEDIA_PLAYER_STATE_PAN_LEFT, *(float*)(_ + MEDIA_PLAYER_STATE_PAN_RIGHT) };
}
float ani::MediaPlayer::get_volume() {
	auto p = get_pan();
	return alib_max(p.left, p.right);
}
void ani::MediaPlayer::setVolume(float volume) { pan(volume, volume); }
long ani::MediaPlayer::durationMsec() {
	jclass mdC = getMediaPlayerClass();
	auto skM = env->GetMethodID(mdC, "getDuration", "()I");
	return env->CallIntMethod(*__thiz, skM);
}
long ani::MediaPlayer::timestampMsec() {
	jclass mdC = getMediaPlayerClass();
	auto skM = env->GetMethodID(mdC, "getCurrentPosition", "()I");
	return env->CallIntMethod(*__thiz, skM);
}
double ani::MediaPlayer::timestamp() {
	long dm = timestampMsec();
	if (dm == 0) { return 0; }
	return dm / 1000.;
}
double ani::MediaPlayer::duration() {
	long dm = durationMsec();
	if (dm == 0) { return 0; }
	return dm / 1000.;
}
void ani::MediaPlayer::prepare() {
	if (*(_ + MEDIA_PLAYER_STATE_PREPARED) == true) {
		LOGW("MediaPlayer has already been prepared!");
		
	}
	jclass mdC = getMediaPlayerClass();
	auto skM = env->GetMethodID(mdC, "prepare", "()V");
	env->CallVoidMethod(*__thiz, skM);
	*(_ + MEDIA_PLAYER_STATE_PREPARED) = true;
}
bool ani::MediaPlayer::isLooping() {
	jclass mdC = getMediaPlayerClass();
	auto skM = env->GetMethodID(mdC, "isLooping", "()Z");
	return env->CallBooleanMethod(*__thiz, skM);
}
void ani::MediaPlayer::setLooping(bool state) {
	jclass mdC = getMediaPlayerClass();
	auto skM = env->GetMethodID(mdC, "setLooping", "(Z)V");
	env->CallVoidMethod(*__thiz, skM, state);
}