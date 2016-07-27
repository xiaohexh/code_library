#include <iostream>

#include <pthread.h>

#include "log4cpp/Category.hh"   
#include "log4cpp/FileAppender.hh"   
#include "log4cpp/PropertyConfigurator.hh"
#include "log4cpp/BasicLayout.hh" 

void use_config_file()
{
	//读取解析配置文件
    log4cpp::PropertyConfigurator::configure("./log4cpp.conf");

    //实例化category对象
    // log4cpp::Category* root = &log4cpp::Category::getRoot();
    log4cpp::Category& root = log4cpp::Category::getRoot();
    log4cpp::Category& main = 
        log4cpp::Category::getInstance(std::string("MAIN"));

    //使用category对象进行日志处理
    main.debug("This is debug");
    main.info("This is info");
    main.alert("This is alert");
}

void without_config_file()
{
    // 1实例化一个layout 对象           
    log4cpp::Layout* layout = new log4cpp::BasicLayout();           
    // 2. 初始化一个appender 对象    
    log4cpp::Appender* appender = new log4cpp::FileAppender("FileAppender", "./test_log4cpp1.log");    
    // 3. 把layout对象附着在appender对象上    
    appender->setLayout(layout);    
    // 4. 实例化一个category对象    
    log4cpp::Category& warn_log = log4cpp::Category::getInstance("mywarn");    
    // 5. 设置additivity为false，替换已有的appender           
    warn_log.setAdditivity(false);    
    // 5. 把appender对象附到category上    
    warn_log.setAppender(appender);    
    // 6. 设置category的优先级，低于此优先级的日志不被记录    
    warn_log.setPriority(log4cpp::Priority::WARN);    
    // 记录一些日志    
    warn_log.info("Program info which cannot be wirten");    
    warn_log.debug("This debug message will fail to write");    
    warn_log.alert("Alert info");    
    // 其他记录日志方式    
    warn_log.log(log4cpp::Priority::WARN, "This will be a logged warning");    
    log4cpp::Priority::PriorityLevel priority;    
    bool this_is_critical = true;    
    if(this_is_critical)     
        priority = log4cpp::Priority::CRIT;    
    else     
        priority = log4cpp::Priority::DEBUG;    
    warn_log.log(priority,"Importance depends on context");      
    warn_log.critStream() << "This will show up << as "   << 1 << " critical message";
    // clean up and flush all appenders  
	log4cpp::Category::shutdown();   

	/* NOTICE:
	 * log4cpp manage object which create by 'new' operator,
	 * so DONOT free them by 'delete',
	 * otherwise will be coredump
	 */
}

int main()
{
	use_config_file();
	return 0;
}