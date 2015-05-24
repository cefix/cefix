/*
 *  NotificationCenter_test.cpp
 *  
 *
 *  Created by Stephan Huber on Tue Nov 18 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/Log.h>
#include <osg/Group>
#include <cefix/NotificationListener.h>
 
 

class test1class : public osg::Referenced, public cefix::NotificationListener {

    public:
        test1class() {
            
                         
             // subscribe mich für messages test1 und test3
             // NotificationFunctor ist eine c++.template-klasse die dazu dient, die dazu dient eine bestimmte
             // Methode eines Objekts aufzurufen (ähnlich einem Callback) er speichert sozusagen Objekt und Handler
             //erzeugt wird dieses mittels:
             // new NotificationFunctor<NamederKlasse<(<Objektreferemz>, <adresseDesHandlers>);
             
             subscribeTo("test1",this,&test1class::handleTest1Message);
             subscribeTo("test3", this,&test1class::handleTest3Message);
            
        }
        
        void handleTest1Message() {
            osg::notify(osg::ALWAYS) << "test1class handling test1 message " << std::endl;
		}
        
        void handleTest3Message(const std::string& str) {
			osg::notify(osg::ALWAYS) << "test1class handling test3 message: " << str << std::endl;
        }
};
        

class test2class : public osg::Referenced, public cefix::NotificationListener {

    public:
        test2class() {
            
                          
             // subscribe mich für messages test1 und test3
             // NotificationFunctor ist eine c++.template-klasse die dazu dient, die dazu dient eine bestimmte
             // Methode eines Objekts aufzurufen (ähnlich einem Callback) er speichert sozusagen Objekt und Handler
             //erzeugt wird dieses mittels:
             // new NotificationFunctor<NamederKlasse<(<Objektreferemz>, <adresseDesHandlers>);
             
             subscribeTo("test2", this,&test2class::handleTest2Message);
             subscribeTo("test3", this,&test2class::handleTest3Message);
			 subscribeTo("test4", this,&test2class::handleTest4Message);
            
        }
        
        void handleTest2Message() 
		{
            osg::notify(osg::ALWAYS) << "test2class handling test2 message" << std::endl;
        }
		
		void handleTest3Message(const std::string& str) 
		{ 
            osg::notify(osg::ALWAYS) << "test2class handling test3 message: " << str << std::endl;
        }
		
		void handleTest4Message(osg::Group* g) 
		{ 
            osg::notify(osg::ALWAYS) << "test2class handling test4 message: " << g->getName() << std::endl;
        }
};

static void notify(const std::string& msg) {

	osg::notify(osg::ALWAYS) << ":::  " << msg << std::endl;

	cefix::NotificationCenter* nc = cefix::NotificationCenter::instance();
	
	nc->notify("test1");
	nc->notify("test2");
	nc->notify("test3", msg);
	
	osg::ref_ptr<osg::Group> group = new osg::Group();
	group->setName("testGroup");
	nc->notify("test4", group.get() );

}



int main(int argc, char *argv[])
{
    osg::setNotifyLevel(osg::DEBUG_INFO);
    
        
    osg::ref_ptr<test1class> t1 = new test1class;
    osg::ref_ptr<test2class> t2 = new test2class;
    
	notify("run one");
   
    t1 = NULL;
	notify("run two");
	
	t2 = NULL;
	notify("run three");
	
	return 0;
}