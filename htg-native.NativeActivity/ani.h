// Android Native Interface : An interface for communicating with Java from native C++ applications built for Android.
// Also includes various ease-of-use libraries (ie. curlpp, http requests, etc.)
// Licensing: See https://github.com/avetharun/avetharun/blob/main/gnu_gpl_3_a_LICENSE
#ifndef __rosemary_project_util_usb_h_
#define __rosemary_project_util_usb_h_
#include <curlpp/cURLpp.hpp>
#include <jni.h>
#include <list>
#include <map>
#include <array>
#include <vector>
#define ani_nofree static inline
#include <variant>
enum class Gravity;
namespace ANIEnv {
	JavaVM* getVM();
	jobject getRootView();
	// Returns a pointer to a new JNIEnv if it's not already initialized.
	void EnsureEnvV(JavaVM* thisvm, JNIEnv* _env);
	// Returns a pointer to a new JNIEnv if it's not already initialized.
	JNIEnv* EnsureEnv(JavaVM* vm);
	JNIEnv* EnsureEnv();
	struct android_app* app();
	jobject appClass();
	int init(JavaVM* __vm, const char* app_namespace, jobject AndroidAppClazz, struct android_app* app);
};
namespace ani {
	///Example reading values
	///SharedPreferences sharedPref(env,context,"pref_telemetry");
	///T_Protocol=sharedPref.getInt(IDT::T_Protocol);
	///Example writing values
	///SharedPreferences_Editor editor=sharedPref.edit();
	///editor.putString("MY_KEY","HELLO");
	///editor.commit();

	class SharedPreferences_Editor {
	public:
		SharedPreferences_Editor(JNIEnv* env, const jobject joSharedPreferences_Edit);
		//return itself for method chaining
		const SharedPreferences_Editor& putBoolean(const char* key, const bool value)const;
		const SharedPreferences_Editor& putInt(const char* key, const int value)const;
		const SharedPreferences_Editor& putString(const char* key, std::string value)const;
		const SharedPreferences_Editor& putLong(const char* key, float value)const;
		const SharedPreferences_Editor& putFloat(const char* key, float value)const;
		bool commit()const;
		void apply()const;
		const SharedPreferences_Editor& clear()const;
		const SharedPreferences_Editor& remove(const char* key)const;
	private:
		JNIEnv* env;
		jobject joSharedPreferences_Edit;
		jmethodID jmPutBoolean;
		jmethodID jmPutFloat;
		jmethodID jmPutInt;
		jmethodID jmPutLong;
		jmethodID jmPutString;
		jmethodID jmCommit;
		jmethodID jmApply;
		jmethodID jmClear;
		jmethodID jmRemove;
	};


	class SharedPreferences {
	public:
		SharedPreferences(SharedPreferences const&) = delete;
		void operator=(SharedPreferences const&) = delete;
	public:
		//Note: Per default, this doesn't keep the reference to the sharedPreferences java object alive
		//longer than the lifetime of the JNIEnv.
		//With keepReference=true the joSharedPreferences is kept 'alive' and you can still use the class after the original JNIEnv* has become invalid -
		//but make sure to refresh the JNIEnv* object with a new valid reference via replaceJNI()
		SharedPreferences(JNIEnv* env, jobject activity_clazz, const char* name, const bool keepReference = false);
		void replaceJNI(JNIEnv* newEnv);
	private:
		JNIEnv* env;
		jobject joSharedPreferences;
		jmethodID jmGetBoolean;
		jmethodID jmGetInt;
		jmethodID jmGetLong;
		jmethodID jmGetFloat;
		jmethodID jmGetString;
		jmethodID jmEdit;
		jmethodID jmContains;
	public:
		bool getBoolean(const char* id, bool defaultValue = false)const;
		int getInt(const char* id, int defaultValue = 0)const;
		float getFloat(const char* id, float defaultValue = 0.0f)const;
		long getLong(const char* id, long defaultValue = 0)const;
		std::string getString(const char* id, const char* defaultValue = "")const;
		void getBoolean(const char* id, bool* out, bool defaultValue = false)const;
		void getInt(const char* id, int* out, int defaultValue = 0)const;
		void getFloat(const char* id, float* out, float defaultValue = 0.0f)const;
		void getLong(const char* id, long* out, long defaultValue = 0)const;
		void getString(const char* id, std::string* out, const char* defaultValue = "")const;
		void getString(const char* id, char* out, size_t out_len, const char* defaultValue = "")const;
		bool contains(std::string id);
		SharedPreferences_Editor edit()const;
	private:
		static constexpr const int  MODE_PRIVATE = 0; //taken directly from java, assuming this value stays constant in java
		static constexpr const int  MODE_WORLD_READABLE = 1; //taken directly from java, assuming this value stays constant in java
		static constexpr const int  MODE_WORLD_WRITEABLE = 2; //taken directly from java, assuming this value stays constant in java
	};

	// end shared prefs
	namespace Networking {
		void openURLBrowser(const char* url);
		CURL* impl_curl_init();
		void ImplRequestInit();
		void sendPostRequestDataImpl__(const char* url, std::string body, std::string* response,
			std::list<std::string> headers = { "Content-Type: text/plain" }
		);
		void ImplGetRequestHandler__(const char* url, std::string* response);
		struct HeaderType;
		// Assumes <response> will be valid upon request finished. Ensure it doesn't get deleted using shared_ptr or a static variable!
		// *response will automatically be cleared before the request is sent.
		void sendGetRequestAsync(const char* url, std::string* response);
		std::string sendGetRequest(const char* url);
		// Assumes <response> will be valid upon request finished. Ensure it doesn't get deleted using shared_ptr or a static variable!
		// *response will automatically be cleared before the request is sent.
		void sendPostRequestAsync(const char* url, std::string body, std::string* response, std::list<std::string> headers = { "Content-Type: text/plain" });
		std::string sendPostRequest(const char* url, std::string body, std::list<std::string> headers = { "Content-Type: text/plain" });
	};
	namespace SystemService {
		jclass GetClass();
		jobject getSystemService(const char* service_type);
	};
	jobject getActivityContext(JNIEnv* env, jobject activity_object);
	jobject getGlobalContext(JNIEnv* env);
	float get_refresh_rate();
	bool adb_enabled();

	int AndroidGenericDimenShown(const char* bar_name, int* height);
	int AndroidStatusBarShown(int* height);
	int AndroidStatusBarHeight();
	int AndroidNavigationBarShown(int* height);
	int AndroidNavigationBarHeight();
	int AndroidKeyboardShown(bool return_height = 0);
	int AndroidKeyboardHeight();
	void AndroidToggleKeyboard();
	int GetUnicodeChar(struct android_app* app, int eventType, int keyCode, int metaState);
	int16_t AndroidGetUnicodeChar(int keyCode, int metaState);
	void AndroidShowKeyboard();
	void AndroidHideKeyboard();
	void AndroidSetWakeLockState(int state);
	float getDPI();
	// Check if the app is being run as Windows Subsystem for Android:tm:
	bool isUsingWSA();
	namespace Build {
		int SDK_INT();
		namespace VERSION_CODES {
			enum {
				BASE = 1,
				BASE_1_1,
				CUPCAKE,
				DONUT,
				ECLAIR,
				ECLAIR_0_1,
				ECLAIR_MR1,
				FROYO,
				GINGERBREAD,
				GINGERBREAD_MR1,
				HONEYCOMB,
				HONEYCOMB_MR1,
				HONEYCOMB_MR2,
				ICE_CREAM_SANDWICH,
				ICE_CREAM_SANDWICH_MR1,
				JELLY_BEAN,
				JELLY_BEAN_MR1,
				JELLY_BEAN_MR2,
				KITKAT,
				KITKAT_WATCH,
				LOLLIPOP,
				LOLLIPOP_MR1,
				MARSHMALLOW, M = MARSHMALLOW,
				N, NOUGAT = N,
				N_MR1, NOUGAT_MR1 = N_MR1,
				O, OREO = O,
				O_MR1, OREO_MR1 = O_MR1,
				P, PIE = P,
				Q, ANDROID_10 = Q,
				R, ANDROID_11 = R,
				S,
				S_V2,
				TIRAMISU,
				CUR_DEVELOPMENT = 10000,
				WINDOWS_SUBSYSTEM_FOR_ANDROID = 10001,
			};
		};
		std::string GetBuildName(int BuildID);
	};
	namespace Vibrator {
		void Vibrate(float millis);
	};
	struct MediaPlayer {
		// Used internally 
		char _[(sizeof(float) * 2) + sizeof(bool)];
		MediaPlayer();
		MediaPlayer(std::string filename, bool fromInsideAPK = false);
		void setDataSource(std::string filename, bool fromInsideAPK = false);
		void prepare();
		void setLooping(bool state);
		bool isLooping();
		bool isPlaying();
		void pause();
		void start();
		void play();
		void stop();
		void restart();
		// Seek to specific point in milliseconds
		void seek(int msec);
		// Seek to specific point in nanoseconds
		void seekNanos(long long uSec);
		void seekMicros(long long micSec);
		// Seek to specific point in seconds
		void seekSeconds(double sec);
		// Sets and overrides the overall volume
		void setVolume(float volume);
		// Sets and overrides the panning volume for each ear.
		void pan(float volume_left, float volume_right);
		struct pan_state_t { float left; float right; };
		// Returns {left=left_volume, right=right_volume};
		pan_state_t get_pan();
		// Returns max(pan_left, pan_right);
		float get_volume();
		long durationMsec();
		// Timestamp (in seconds)
		double timestamp();
		// Timestamp (in milliseconds)
		long timestampMsec();
		// Duration (in seconds)
		double duration();
		std::shared_ptr<jobject> __thiz;
		void operator~();
	};
};

#endif