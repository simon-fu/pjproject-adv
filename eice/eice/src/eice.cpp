
#include <stdio.h>
#include <string>
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjnath.h>
#include <string.h>
#include <json/json.h>
#include <eice.h>


#define dbgi(...) PJ_LOG(3, (__FILE__, __VA_ARGS__))
#define dbgw(...) PJ_LOG(2, (__FILE__, __VA_ARGS__))
#define dbge(...) PJ_LOG(1, (__FILE__, __VA_ARGS__))

#define KEEP_ALIVE_INTERVAL 300
#define MAX_COMPONENT_COUNT 4
#define MAX_CANDIDATE_COUNT 8
#define MAX_IP_SIZE 64

struct eice_context{
	int pj_inited;
	int pjlib_inited;
	int pjnath_inited;
	int pjlog_installed;
	FILE *log_fhnd;
    
    pj_caching_pool cp;
    int cp_inited;
    pj_pool_t * pool;
};

class eice_config{
public:
	std::string turn_host; // default turn server
	int turn_port;
    
//    std::string turn_all_hosts[PJ_STUN_SOCK_MAX_STUN_NUM]; // all turn servers
//    int turn_all_port[PJ_STUN_SOCK_MAX_STUN_NUM];
//    int turn_num;
    
    stun_bind_cfg bind_cfg;
    
	int comp_count;
};

struct eice_st{
	eice_config * cfg;

    // ice instance
    pj_caching_pool cp;
    int cp_inited;

    pj_pool_t *ice_pool;


    pj_lock_t	    *lock;

    pj_ice_strans_cfg ice_cfg;
    pj_ice_strans *icest;
    pj_ice_sess_role role;

    int ice_init_done;
    pj_status_t ice_init_result;

    int ice_nego_done;
    pj_status_t ice_nego_status;


    // callback thread.
    pj_thread_t *ice_thread;
    char thread_quit_flag;

//    char local_ufrag[512];
//    char local_pwd[512];

    std::string * local_content;
    std::string * remote_content;

    pj_ice_sess_cand remote_cands[MAX_CANDIDATE_COUNT];
    int remote_cand_count;

    
    char err_str[1024];

};



static struct eice_context g_eice_stor;
static struct eice_context * g_eice = 0;

static int eice_worker_thread(void *arg);
static pj_status_t handle_events(eice_t obj, unsigned max_msec, unsigned *p_count);

//#define REGISTER_THREAD pj_thread_t *pthread = NULL; \
//pj_thread_desc desc; \
//pj_bool_t has_registered = PJ_FALSE; \
//has_registered = pj_thread_is_registered(); \
//if(!has_registered) { \
//if (pj_thread_register(NULL, desc, &pthread) == PJ_SUCCESS) { \
//} \
//}

static void register_eice_thread(pj_pool_t * pool){
    pj_thread_t *pthread = NULL;
//    pj_thread_desc desc;
    pj_thread_desc * pdesc ;
    pj_bool_t has_registered = PJ_FALSE;
    has_registered = pj_thread_is_registered();
    if(has_registered) return;
    
    pj_size_t sz =  sizeof(pj_thread_desc);
    pdesc = (pj_thread_desc *)pj_pool_alloc(pool,sz);
    
    if (pj_thread_register(NULL, *pdesc, &pthread) != PJ_SUCCESS) {
        dbge(" **** register thread ERROR ****");
    }
}


static void log_func_4pj(int level, const char *data, int len) {

//    register_eice_thread(g_eice->pool);
    
    if(!g_eice) return;

    //pj_log_write(level, data, len);
    printf("%s", data);
}


#define dbgraw(x) printf(x "\n")

int eice_init()
{
    
    int ret = -1;
    dbgraw("eice_init");
    
	if (g_eice) {
		ret = 0;
		dbgraw("eice is already initialized, return ok directly.");
		return 0;
	}

   
    
	g_eice = &g_eice_stor;
	memset(g_eice, 0, sizeof(struct eice_context));
    dbgraw("memset OK");


    do {
        
        ret = pj_init();
        if (ret != PJ_SUCCESS) {
            dbgraw("pj_init failure ");
            break;
        }
        g_eice->pj_inited = 1;
        dbgraw("pj_init OK");
        
        
        // create pool factory
        pj_caching_pool_init(&g_eice->cp, NULL, 0);
        g_eice->cp_inited = 1;
        dbgraw("pj_caching_pool_init OK");
        
        /* Create application memory pool */
        g_eice->pool = pj_pool_create(&g_eice->cp.factory, "eice_global_pool",
                                      512, 512, NULL);
        dbgraw("pj_pool_create OK");
        
        register_eice_thread(g_eice->pool);
        dbgraw("register_eice_thread  OK");
        
        pj_log_set_log_func(&log_func_4pj);
        dbgraw("pj_log_set_level OK");
        
        pj_log_set_level(PJ_LOG_MAX_LEVEL);
        dbgraw("pj_log_set_level OK");
        
        
        ret = pjlib_util_init();
        if (ret != PJ_SUCCESS) {
            dbge("pjlib_util_init failure, ret=%d", ret);
            break;
        }
        g_eice->pjlib_inited = 1;
        dbgraw("pjlib_util_init OK");
        
        ret = pjnath_init();
        if (ret != PJ_SUCCESS) {
            dbge("pjnath_init failure, ret=%d", ret);
            break;
        }
        g_eice->pjnath_inited = 1;
        dbgraw("pjnath_init OK");
        

        
        ret = PJ_SUCCESS;
        dbgi("eice init ok");

    } while (0);

    if(ret != PJ_SUCCESS){
    	eice_exit();
    }
    return ret;
}

void eice_exit() {

	if(!g_eice) return;
    
//    if(g_eice->pool){
//        register_eice_thread(g_eice->pool);
//    }
    
    if(g_eice->cp_inited){
        pj_caching_pool_destroy(&g_eice->cp);
        g_eice->cp_inited = 0;
    }
    

	if(g_eice->pj_inited){
		pj_shutdown();
		g_eice->pj_inited = 0;
	}

	dbgi("eice exit ok");
	g_eice = 0;
}

static std::string _json_get_string(Json::Value& request, const char * name,
		const std::string& default_value) {
	if (request[name].isNull()) {
		dbgw("name %s NOT found in json!!!", name);
		return default_value;
	}

	if (!request[name].isString()) {
		dbgw("name %s is NOT string in json!!!", name);
		return default_value;
	}
	return request[name].asString();
}

static int _json_get_int(Json::Value& request, const char * name,
		unsigned int default_value) {
	if (request[name].isNull()) {
		dbgw("name %s NOT found in json!!!", name);
		return default_value;
	}

	if (!request[name].isUInt()) {
		dbgw("name %s is NOT UInt in json!!!", name);
		return default_value;
	}
	return request[name].asInt();
}

static
const Json::Value& _json_get_array(Json::Value& request, const char * name, const Json::Value& default_value)
{
	if(request[name].isNull())
	{
		dbgw("name %s NOT found in json!!!", name);
		return default_value;
	}

	if(!request[name].isArray())
	{
		dbgw("name %s is NOT array in json!!!", name);
		return default_value;
	}
	return request[name];
}

static eice_config * parse_eice_config(eice_t obj, const char * config_json){
	int ret = -1;
	eice_config * cfg = new eice_config;
	Json::Reader reader;
	Json::Value value;

	do {
		if(!config_json){
			config_json = "{}";
		}

		if (!reader.parse(config_json, value)) {
			dbge("parse config JSON fail!!!");
			ret = -1;
			break;
		}

		cfg->turn_host = _json_get_string(value, "turnHost", std::string(""));
		cfg->turn_port = _json_get_int(value, "turnPort", 3478);
		cfg->comp_count = _json_get_int(value, "compCount", 2);
        
//        cfg->turn_num = 0;
        cfg->bind_cfg.stun_serv_num = 0;
        cfg->bind_cfg.stun_bind_timeout = 3000;
        Json::Value turnAddrs = _json_get_array(value, "turnAddrs",Json::Value());
        if (!turnAddrs.empty()) {
            dbgi("turn addr num %d", turnAddrs.size());
            for(unsigned int ui = 0; ui < turnAddrs.size(); ui++){
                Json::Value json_addr_value = turnAddrs[ui];
                std::string host = _json_get_string(json_addr_value, "host", std::string(""));
                int port = _json_get_int(json_addr_value, "port", 3478);
//                cfg->turn_all_hosts[ui] = host;
//                cfg->turn_all_port[ui] = port;
                pj_strdup2_with_null(obj->ice_pool, &cfg->bind_cfg.stun_serv_ips[ui], host.c_str());
                cfg->bind_cfg.stun_serv_ports[ui] = port;
            }
//            cfg->turn_num = turnAddrs.size();
            cfg->bind_cfg.stun_serv_num = turnAddrs.size();
        }
        
        if(cfg->turn_host.empty() && turnAddrs.size() > 0){
            cfg->turn_host = std::string(cfg->bind_cfg.stun_serv_ips[0].ptr, cfg->bind_cfg.stun_serv_ips[0].slen);
            cfg->turn_port = cfg->bind_cfg.stun_serv_ports[0];
        }
        
        ret = 0;
	} while (0);

	if(ret != 0){
		delete cfg;
		cfg = 0;
	}
	return cfg;
}

static void dump_eice_config(eice_config * cfg){
    dbgi("=== eice_config ===>");
	dbgi("defalut turn server: %s:%d", cfg->turn_host.c_str(), cfg->turn_port);
	dbgi("compCount: %d", cfg->comp_count);
    dbgi("turnAddrs num: %d", cfg->bind_cfg.stun_serv_num);
    for(int i = 0; i < cfg->bind_cfg.stun_serv_num; i++){
        dbgi("turnAddrs[%d]: %s:%d", i, cfg->bind_cfg.stun_serv_ips[i].ptr, cfg->bind_cfg.stun_serv_ports[i]);
    }
    dbgi("<=== eice_config ===");
}


static void cb_on_ice_complete(pj_ice_strans *ice_st, pj_ice_strans_op op,
		pj_status_t status) {

//    register_eice_thread(g_eice->pool);

    dbge("cb_on_ice_complete: op=%d", op);
    

	switch (op) {
	case PJ_ICE_STRANS_OP_INIT: {
		dbgi("ice init result : %s", status == PJ_SUCCESS ? "OK" : "FAIL");
		eice_t obj = (eice_t) pj_ice_strans_get_user_data(ice_st);
		pj_lock_acquire(obj->lock);
		obj->ice_init_done = 1;
		obj->ice_init_result = status;
		pj_lock_release(obj->lock);
	}
		break;

	case PJ_ICE_STRANS_OP_NEGOTIATION: {
		dbgi("ice nego result : %s", status == PJ_SUCCESS ? "OK" : "FAIL");
		eice_t obj = (eice_t) pj_ice_strans_get_user_data(ice_st);
		pj_lock_acquire(obj->lock);
		obj->ice_nego_done = 1;
		obj->ice_nego_status = status;
		pj_lock_release(obj->lock);
	}
		break;
	default: {
		dbge("unknown operation:%d", op);
	}
		break;
	} // end of switch
}

static int query_wait(pj_lock_t * lock, int * pflag, int * pcancel, unsigned int timeout_ms){

	pj_timestamp ts_start;
	pj_timestamp ts;
	int ret = -1;
	int cancel =  0;

	dbgi("before pcancel=%p", pcancel);
	if(!pcancel) {
		pcancel = &cancel;
	}
	dbgi("after pcancel=%p", pcancel);

	pj_lock_acquire(lock);
	pj_get_timestamp(&ts_start);
	ts = ts_start;
	while (!(*pflag) && !(*pcancel)) {
//		unsigned int elapse = (unsigned ) (ts.u64 - ts_start.u64);
		pj_uint32_t elapse = pj_elapsed_msec(&ts_start, &ts);
		dbgi("elapsed=%d msec", elapse);
		if(elapse >= timeout_ms) break;
        pj_lock_release(lock);
        
		pj_thread_sleep(50);
		pj_get_timestamp(&ts);
		pj_lock_acquire(lock);
	}
	dbgi("(*pflag)=%d", (*pflag));
	ret = (*pflag) != 0 ? 0 : -1;
	pj_lock_release(lock);
	return ret;
}

static void get_ip_port_from_sockaddr(const pj_sockaddr *addr, char *address, int *port) {

    if(pj_sockaddr_has_addr(addr)){
        *port = (int) pj_sockaddr_get_port(addr);
        pj_sockaddr_print(addr, address, MAX_IP_SIZE, 0);
    }else{
        *port = 0;
        address[0] = '\0';
    }
    
}

static void cand_to_json_value(int comp_id, int cand_id, const pj_ice_sess_cand * cand, Json::Value& json_val_cand){
	char ip[MAX_IP_SIZE];
	int port;
	char base_ip[MAX_IP_SIZE];
	int base_port;

	get_ip_port_from_sockaddr(&cand->addr, ip, &port);
	get_ip_port_from_sockaddr(&cand->base_addr, base_ip, &base_port);
//	dbgi("\t cand[%d,%d]: addr=%s:%d, base_addr=%s:%d", comp_id, cand_id, ip, port, base_ip, base_port);

	json_val_cand["component"] = cand->comp_id;
	json_val_cand["foundation"] = std::string(cand->foundation.ptr,
			cand->foundation.slen);

	json_val_cand["generation"] = "0";
	json_val_cand["network"] = "1";

	json_val_cand["id"] = cand_id; // to be check
	json_val_cand["ip"] = ip;
	json_val_cand["port"] = port;
	json_val_cand["priority"] = cand->prio;

	json_val_cand["protocol"] = "udp";
	json_val_cand["type"] = pj_ice_get_cand_type_name(cand->type);
	if (cand->type != PJ_ICE_CAND_TYPE_HOST) {
		if (pj_sockaddr_has_addr(&cand->base_addr)) {
			json_val_cand["rel-addr"] = base_ip;
			json_val_cand["rel-port"] = base_port;
		} else {
			json_val_cand["rel-addr"] = ip;
			json_val_cand["rel-port"] = port;
		}
	}
}

int eice_new(const char* config, pj_ice_sess_role role, eice_t * pobj) {

	int ret = -1;
	eice_t obj = 0;
	eice_config * cfg = 0;

	do {
		obj = (eice_t) malloc(sizeof(struct eice_st));
        memset(obj, 0, sizeof(struct eice_st));

		
		
		obj->role = role;

		// create pool factory
		pj_caching_pool_init(&obj->cp, NULL, 0);
		obj->cp_inited = 1;

		/* Init our ICE settings with null values */
		pj_ice_strans_cfg_default(&obj->ice_cfg);

		// set caching pool factory
		obj->ice_cfg.stun_cfg.pf = &obj->cp.factory;

		/* Create application memory pool */
		obj->ice_pool = pj_pool_create(&obj->cp.factory, "eice_pool",
				512, 512, NULL);
        
        cfg = parse_eice_config(obj, config);
        if(!cfg){
            ret = -1;
            break;
        }
        dump_eice_config(cfg);
        obj->cfg = cfg;

		ret = pj_lock_create_recursive_mutex(obj->ice_pool, NULL, &obj->lock);
		if (ret != PJ_SUCCESS) {
			dbge("failed to create lock, ret=%d", ret);
			break;
		}


		/* Create timer heap for timer stuff */
		ret = pj_timer_heap_create(obj->ice_pool, 100,
				&obj->ice_cfg.stun_cfg.timer_heap);
		if (ret != PJ_SUCCESS) {
			dbge("failed to create timer heap, ret=%d", ret);
			break;
		}

		/* and create ioqueue for network I/O stuff */
		ret = pj_ioqueue_create(obj->ice_pool, 16, &obj->ice_cfg.stun_cfg.ioqueue);
		if (ret != PJ_SUCCESS) {
			dbge("failed to create ioqueue, ret=%d", ret);
			break;
		}

		/* something must poll the timer heap and ioqueue,
		 * unless we're on Symbian where the timer heap and ioqueue run
		 * on themselves.
		 */
		ret = pj_thread_create(obj->ice_pool, "eice_thread",
				&eice_worker_thread, obj, 0, 0, &obj->ice_thread);
		if (ret != PJ_SUCCESS) {
			dbge("failed to create worker thread, ret=%d", ret);
			break;
		}

		obj->ice_cfg.af = pj_AF_INET();


		// stun server
		if (!cfg->turn_host.empty()) {
			/* Maximum number of host candidates */
			obj->ice_cfg.stun.max_host_cands = 1;

			/* Nomination strategy */
			obj->ice_cfg.opt.aggressive = PJ_TRUE;


			/* Configure STUN/srflx candidate resolution */
			pj_str_t srv = pj_str((char *)cfg->turn_host.c_str());
			pj_strassign(&obj->ice_cfg.stun.server, &srv);
			obj->ice_cfg.stun.port = (pj_uint16_t)  cfg->turn_port;

			/* stun keep alive interval */
			obj->ice_cfg.stun.cfg.ka_interval = KEEP_ALIVE_INTERVAL;
		} else {
			dbgi("no stun server info provided.");
		}

		// ice callback
		pj_ice_strans_cb icecb;
		pj_bzero(&icecb, sizeof(icecb));
		icecb.on_ice_complete = cb_on_ice_complete;

		// create ice strans instance;
        if(obj->cfg->bind_cfg.stun_serv_num == 0){
            ret = pj_ice_strans_create(NULL, &obj->ice_cfg,
                                cfg->comp_count, obj, &icecb,
                                &obj->icest);
        }else{
            ret = pj_ice_strans_create_ext(NULL, &obj->ice_cfg,
                                cfg->comp_count, obj, &icecb,
                                &obj->cfg->bind_cfg,
                                &obj->icest);
        }
		
		if (ret != PJ_SUCCESS) {
            pj_str_t es = pj_strerror(ret, obj->err_str, sizeof(obj->err_str));
			dbge("error creating ice strans, ret=%d(%s)", ret, es.ptr);
			break;
		}

		ret = query_wait(obj->lock, &obj->ice_init_done, NULL, 30*1000);
		if (ret != 0) {
			dbge("wait ice init timeout!!!");
			break;
		}

		// create ice session after ice strans inited;
		ret = pj_ice_strans_init_ice(obj->icest, obj->role, 0, 0);
        if (ret != PJ_SUCCESS) {
            dbge("error init ice session, ret=%d", ret);
            break;
        }
        

//		dbgi("sleeping 2000 ms");
//		pj_thread_sleep(2000);

		// get local ufrag and pwd

		pj_str_t loc_ufrag;
		pj_str_t loc_pwd;
		ret = pj_ice_strans_get_ufrag_pwd(obj->icest, &loc_ufrag, &loc_pwd, NULL, NULL); // ignore remote.
		if (ret != PJ_SUCCESS) {
			dbge("error get ufrag and pwd, ret=%d", ret);
			break;
		}

//		strncpy(obj->local_ufrag, loc_ufrag.ptr, loc_ufrag.slen);
//		obj->local_ufrag[loc_ufrag.slen] = 0;
//		strncpy(obj->local_pwd, loc_pwd.ptr, loc_pwd.slen);
//		obj->local_pwd[loc_pwd.slen] = 0;
//		dbgi("local ufrag: %s", obj->local_ufrag);
//		dbgi("local pwd: %s", obj->local_pwd);


		std::string str_local_ufrag(loc_ufrag.ptr, loc_ufrag.slen);
		std::string str_local_pwd(loc_pwd.ptr, loc_pwd.slen);
		dbgi("local ufrag: %s, len=%d", str_local_ufrag.c_str(), loc_ufrag.slen);
		dbgi("local pwd: %s, len=%d", str_local_pwd.c_str(), loc_pwd.slen);



		// get local candidates
		Json::Value json_val_cands;
		ret = PJ_SUCCESS;
		for(int comp = 1; comp <= cfg->comp_count; comp++){
			unsigned candCount = MAX_CANDIDATE_COUNT;
			pj_ice_sess_cand cands[MAX_CANDIDATE_COUNT];
			memset(cands, 0, sizeof(pj_ice_sess_cand) * MAX_CANDIDATE_COUNT);

			ret = pj_ice_strans_enum_cands(obj->icest, comp, &candCount, cands);
			if (ret != PJ_SUCCESS) {
				dbge("error get candidates of comp %d", comp);
				break;
			}
			dbgi("comp %d has candidates %d", comp, candCount);

			for(int ci = 0; ci < candCount; ci++){
				pj_ice_sess_cand * cand = &cands[ci];

				// make candidate JSON
				Json::Value json_val_cand;
				cand_to_json_value(comp, ci, cand, json_val_cand);

				json_val_cands.append(json_val_cand);

			}
		}
		if (ret != PJ_SUCCESS) break;

		Json::Value json_val;
		json_val.clear();
		json_val["ufrag"] = str_local_ufrag;
		json_val["pwd"] = str_local_pwd;
		json_val["candidates"] = json_val_cands;

		Json::FastWriter writer;
		obj->local_content = new std::string(writer.write(json_val));

		dbgi("local-content= %s\n", obj->local_content->c_str());


//		std::string sytled_str = json_val.toStyledString();
//		dbgi("styled-local-content=  %s\n", sytled_str.c_str());

		*pobj = obj;
		ret = 0;

	} while (0);

	if(ret != 0){
		eice_free(obj);
	}

	return ret;
}



void eice_free(eice_t obj){
	if(!obj) return ;

    register_eice_thread(g_eice->pool);

	if(obj->local_content){
		delete obj->local_content;
		obj->local_content = 0;
	}

	if (obj->remote_content) {
		delete obj->remote_content;
		obj->remote_content = 0;
	}
    
    
    // stop strans
    if (obj->icest) {

    	// stop session
		if (pj_ice_strans_has_sess(obj->icest)) {
			pj_ice_strans_stop_ice(obj->icest);
		}

        pj_ice_strans_destroy(obj->icest);
        obj->icest = 0;
    }


    // stop thread
    obj->thread_quit_flag = PJ_TRUE;
    if (obj->ice_thread) {
        pj_thread_join(obj->ice_thread);
        pj_thread_destroy(obj->ice_thread);
        obj->ice_thread = 0;
    }


    // stop io queue
    if (obj->ice_cfg.stun_cfg.ioqueue) {
        pj_ioqueue_destroy(obj->ice_cfg.stun_cfg.ioqueue);
        obj->ice_cfg.stun_cfg.ioqueue = 0;
    }


    // stop timer
    if (obj->ice_cfg.stun_cfg.timer_heap) {
        pj_timer_heap_destroy(obj->ice_cfg.stun_cfg.timer_heap);
        obj->ice_cfg.stun_cfg.timer_heap = 0;
    }

    if (obj->lock) {
		pj_lock_destroy(obj->lock);
		obj->lock = 0;
	}


    // stop caching pool
    if(obj->cp_inited){
    	pj_caching_pool_destroy(&obj->cp);
    	obj->cp_inited = 0;
    }

	free(obj);
}


int eice_new_caller(const char* config, char * local_content,
		int * p_local_content_len, eice_t * pobj) {
	int ret = -1;
	eice_t obj = 0;
    
    register_eice_thread(g_eice->pool);
    
	do{
		ret = eice_new(config, PJ_ICE_SESS_ROLE_CONTROLLING, &obj);
		if(ret){
			break;
		}


//        dbgi("obj->local_content = %p", obj->local_content);
//        dbgi("obj->local_content->size() = %d", obj->local_content->size());
//        dbgi("local_content = %p", local_content);
//        dbgi("p_local_content_len = %p", p_local_content_len);
        

		strcpy(local_content, obj->local_content->c_str());
		*p_local_content_len = obj->local_content->size();

		*pobj = obj;
	}while(0);

	if(ret != 0){
		eice_free(obj);
	}
	return ret;
}

static void dump_cand(pj_ice_sess_cand * cand, const char * prefix){
	char ip[MAX_IP_SIZE];
	int port;
	char rel_ip[MAX_IP_SIZE];
	int rel_port;

	get_ip_port_from_sockaddr(&cand->addr, ip, &port);
	get_ip_port_from_sockaddr(&cand->rel_addr, rel_ip, &rel_port);

	dbgi("%s: comp_id=%d, foundation=%s, prio=%d, typ=%s, addr=%s:%d, rel-addr=%s:%d"
						, prefix, cand->comp_id, cand->foundation.ptr, cand->prio, pj_ice_get_cand_type_name(cand->type)
						, ip, port, rel_ip, rel_port);
}

static int parse_content(eice_t obj, const char * content, int content_len
		, std::string& ufrag, std::string& pwd, pj_ice_sess_cand cands[], int& cand_count){
	int ret = -1;
	do{
		Json::Reader reader;
		Json::Value value;
		if (!reader.parse(std::string(content, content_len),
				value)) {
			dbge("parse content JSON fail!!!\n");
			ret = -1;
			break;
		}
		ret = 0;
		ufrag = _json_get_string(value, "ufrag", std::string(""));
		pwd = _json_get_string(value, "pwd", std::string(""));
		Json::Value candidates = _json_get_array(value, "candidates",
				Json::Value());
		if (ufrag.empty()) {
			dbge("ufrag empty !!!\n");
			ret = -1;
		}
		if (pwd.empty()) {
			dbgw("pwd empty !!!\n");
			//			ret = -1;
		}
		if (candidates.empty()) {
			dbge("candidates empty !!!\n");
			ret = -1;
		}
		if (ret != 0)
			break;

		for (int i = 0; i < candidates.size(); i++) {
			Json::Value json_val_cand = candidates[i];
			pj_ice_sess_cand * cand = &cands[i];

			memset(cand, 0, sizeof(pj_ice_sess_cand));

			ret = -1;
			cand->comp_id = _json_get_int(json_val_cand, "component", -1);
			if (cand->comp_id <= 0) {
				dbge("error comp_id %d", cand->comp_id);
				break;
			}

			std::string founda = _json_get_string(json_val_cand, "foundation", std::string(""));
			//pj_cstr(&cand->foundation, founda.c_str());
            pj_strdup2_with_null(obj->ice_pool, &cand->foundation, founda.c_str());
			cand->prio = _json_get_int(json_val_cand, "priority", -1);

			std::string typ = _json_get_string(json_val_cand, "type",
					std::string(""));
			if (typ == "host") {
				cand->type = PJ_ICE_CAND_TYPE_HOST;
			} else if (typ == "srflx") {
				cand->type = PJ_ICE_CAND_TYPE_SRFLX;
			} else if (typ == "relay") {
				cand->type = PJ_ICE_CAND_TYPE_RELAYED;
			} else {
				dbge("unknown candidate type: %s", typ.c_str());
				break;
			}

			std::string ip = _json_get_string(json_val_cand, "ip",
					std::string(""));
			int port = _json_get_int(json_val_cand, "port", 0);
			pj_str_t str;
			//pj_cstr(&str, ip.c_str());
            pj_strdup2_with_null(obj->ice_pool, &str, ip.c_str());
			pj_sockaddr_init(pj_AF_INET(), &cand->addr, &str, port);

			std::string rel_addr = _json_get_string(json_val_cand, "rel-addr",
					std::string(""));
			int rel_port = _json_get_int(json_val_cand, "rel-port", 0);

			pj_str_t pj_str_reladdr;
//			pj_cstr(&pj_str_reladdr, rel_addr.c_str());
            pj_strdup2_with_null(obj->ice_pool, &pj_str_reladdr, rel_addr.c_str());
			pj_sockaddr_init(pj_AF_INET(), &cand->rel_addr, &pj_str_reladdr,
					rel_port);

			//std::string proto = _json_get_string(value, "protocol", std::string(""));

			ret = 0;

			dump_cand(cand, "remote cand");
		}
		if(ret !=0 ) break;
		cand_count = candidates.size();
	}while(0);
	return ret;
}

int eice_start_nego(eice_t obj, const char * remote_content, int remote_content_len){
	int ret = -1;
	do {
		// parse remote content
		std::string ufrag;
		std::string pwd;
		ret = parse_content(obj, remote_content, remote_content_len, ufrag, pwd,
				obj->remote_cands, obj->remote_cand_count);
		if (ret != 0) {
			dbge("parse remote content fail !!!");
			break;
		}

		pj_str_t rufrag, rpwd;
//		pj_cstr(&rufrag, ufrag.c_str());
        pj_strdup2_with_null(obj->ice_pool, &rufrag, ufrag.c_str());
        
//		pj_cstr(&rpwd, pwd.c_str());
        pj_strdup2_with_null(obj->ice_pool, &rpwd, pwd.c_str());

		ret = pj_ice_strans_start_ice(obj->icest, &rufrag, &rpwd,
				obj->remote_cand_count, obj->remote_cands);
		if (ret != 0) {
			dbge("start ice fail !!!");
			break;
		}
		dbgi("start ice OK");
	} while (0);
	return ret;
}

int eice_new_callee(const char* config, const char * remote_content, int remote_content_len,
		char * local_content, int * p_local_content_len,
		eice_t * pobj){
	int ret = -1;
	eice_t obj = 0;
    
    register_eice_thread(g_eice->pool);
	do {
		ret = eice_new(config, PJ_ICE_SESS_ROLE_CONTROLLED, &obj);
		if (ret) {
			break;
		}

		ret = eice_start_nego(obj, remote_content, remote_content_len);
		if(ret != 0){
			dbge("callee start nego fail !!!");
			break;
		}
		dbgi("callee start nego OK");

		strcpy(local_content, obj->local_content->c_str());
		*p_local_content_len = obj->local_content->size();

		*pobj = obj;
	} while (0);

	if (ret != 0) {
		eice_free(obj);
	}
	return ret;
}

int eice_caller_nego(eice_t obj, const char * remote_content, int remote_content_len,
		eice_on_nego_result_t cb, void * cbContext )
{
	int ret = -1;
    
    register_eice_thread(g_eice->pool);
    
	do{
		ret = eice_start_nego(obj, remote_content, remote_content_len);
		if (ret != 0) {
			dbge("caller start nego fail !!!");
			break;
		}
		dbgi("caller start nego OK");
		ret = 0;
	} while (0);
	return ret;
}

int eice_get_nego_result(eice_t obj, char * nego_result, int * p_nego_result_len){
	int nego_done=0;
	pj_status_t nego_status;
    
    register_eice_thread(g_eice->pool);
    
	pj_lock_acquire(obj->lock);
	nego_done = obj->ice_nego_done;
	nego_status = obj->ice_nego_status;
	pj_lock_release(obj->lock);

	if(!nego_done) return -1;

	Json::Value root_val;
	if (nego_status == PJ_SUCCESS) {
		root_val["result"] = 0;

		Json::Value pairs_val;
		for (int comp_id = 1; comp_id <= obj->cfg->comp_count; comp_id++) {
			const pj_ice_sess_check * sess_check = pj_ice_strans_get_valid_pair(
					obj->icest, comp_id);
			if ((sess_check != NULL) && (sess_check->nominated == PJ_TRUE)) {

				Json::Value local_val;
				cand_to_json_value(comp_id, 0, sess_check->lcand, local_val);

				Json::Value remote_val;
				cand_to_json_value(comp_id, 0, sess_check->rcand, remote_val);

				Json::Value pair_val;
				pair_val["comp_id"] = comp_id;
				pair_val["local"] = local_val;
				pair_val["remote"] = remote_val;
				pairs_val.append(pair_val);
			}
		}
		root_val["pairs"] = pairs_val;
	} else {
		root_val["result"] = -1;
	}

	Json::FastWriter writer;
	std::string str = writer.write(root_val);
	dbgi("nego result = %s @@@\n", root_val.toStyledString().c_str());

	strcpy(nego_result, str.c_str());
	*p_nego_result_len = (int)str.size();

	return 0;
}

static pj_status_t handle_events(eice_t obj, unsigned max_msec, unsigned *p_count) {

//    register_eice_thread(g_eice->pool);

    enum {
        MAX_NET_EVENTS = 1
    };
    pj_time_val max_timeout = {0, 0};
    pj_time_val timeout = { 0, 0};
    unsigned count = 0, net_event_count = 0;
    int c;

    max_timeout.msec = max_msec;

    /* Poll the timer to run it and also to retrieve the earliest entry. */
    timeout.sec = timeout.msec = 0;
    c = pj_timer_heap_poll( obj->ice_cfg.stun_cfg.timer_heap, &timeout );
    if (c > 0) {
        count += c;
    }

    /* timer_heap_poll should never ever returns negative value, or otherwise
     * ioqueue_poll() will block forever!
     */
    pj_assert(timeout.sec >= 0 && timeout.msec >= 0);
    if (timeout.msec >= 1000) {
        timeout.msec = 999;
    }

    /* compare the value with the timeout to wait from timer, and use the
     * minimum value.
     */
    if (PJ_TIME_VAL_GT(timeout, max_timeout)) {
        timeout = max_timeout;
    }

    /* Poll ioqueue.
     * Repeat polling the ioqueue while we have immediate events, because
     * timer heap may process more than one events, so if we only process
     * one network events at a time (such as when IOCP backend is used),
     * the ioqueue may have trouble keeping up with the request rate.
     *
     * For example, for each send() request, one network event will be
     *   reported by ioqueue for the send() completion. If we don't poll
     *   the ioqueue often enough, the send() completion will not be
     *   reported in timely manner.
     */
    do {
        c = pj_ioqueue_poll( obj->ice_cfg.stun_cfg.ioqueue, &timeout);
        if (c < 0) {
            pj_status_t err = pj_get_netos_error();
            pj_thread_sleep((unsigned int)PJ_TIME_VAL_MSEC(timeout));
            if (p_count)
                *p_count = count;
            return err;
        } else if (c == 0) {
            break;
        } else {
            net_event_count += c;
            timeout.sec = timeout.msec = 0;
        }
    } while (c > 0 && net_event_count < MAX_NET_EVENTS);

    count += net_event_count;
    if (p_count) {
        *p_count = count;
    }

    return PJ_SUCCESS;

}

static int eice_worker_thread(void *arg) {

	eice_t obj = (eice_t) arg;

//    register_eice_thread(g_eice->pool);

    while (!obj->thread_quit_flag) {
        handle_events(obj, 500, NULL);
    }
    
    dbgi("eice_worker_thread exited");

    return 0;
}

void eice_set_log_func(eice_log_func * log_func){
    pj_log_set_log_func(log_func);
}

void test_log_func(int level, const char *data, int len){
//    printf("%s", data);
}

int eice_test(){
//    new InetSocketAddress("203.195.185.236", 3488), // turn1
//    new InetSocketAddress("121.41.75.10", 3488), // turn3
    
//    const char * config_json = "{\"turnHost\":\"203.195.185.236\",\"turnPort\":3488,\"compCount\":2}";
//    const char * config_json = "{\"turnHost\":\"203.195.185.36\",\"turnPort\":3488,\"compCount\":2}"; // error ip
    
    const char * config_json = "{"
//        "\"turnHost\":\"203.195.185.236\",\"turnPort\":3488,"
        "\"compCount\":2,"
        "\"turnAddrs\":["
            "{\"host\":\"203.195.185.236\",\"port\":3488}," // turn1
            "{\"host\":\"121.41.75.10\",\"port\":3488}" // turn3
            "]"
        "}";
    

    int ret = 0;

    eice_init();
    
//    eice_set_log_func(test_log_func);
    
    char * caller_content = new char[8*1024];
    int caller_content_len = 0;
    eice_t caller = 0;
    ret = eice_new_caller(config_json, caller_content, &caller_content_len, &caller);

    char * callee_content = new char[8*1024];
	int callee_content_len = 0;
	eice_t callee = 0;
    ret = eice_new_callee(config_json, caller_content, caller_content_len, callee_content, &callee_content_len, &callee);

    ret = eice_caller_nego(caller, callee_content, callee_content_len, 0, 0);

    char * caller_result = new char[8*1024];
    int caller_result_len = 0;
    char * callee_result = new char[8*1024];
    int callee_result_len = 0;
    while(caller_result_len == 0 && callee_result_len == 0){
    	if(caller_result_len == 0){
    		ret = eice_get_nego_result(caller, caller_result, &caller_result_len);
    	}
    	if(callee_result_len == 0){
    		ret = eice_get_nego_result(callee, callee_result, &callee_result_len);
    	}
    	pj_thread_sleep(50);
    }

    eice_free(caller);
    eice_free(callee);
    
    //ret = eice_new_caller(config_json, caller_content, &caller_content_len, &caller);
    
    
    eice_exit();
    
    delete []caller_result;
    delete []caller_content;
    
    delete []callee_result;
    delete []callee_content;
    
    return ret;
}






