#ifndef OFP_FLOWMODFIELD_H
#define OFP_FLOWMODFIELD_H


class FlowModField {
public:
	
	template <class T, int N>
	T get();
		
	template<>
	oxm_ipv4_src get<OXM_IPV4_SRC>();
	
	
};

OXM_ipv4_src()




#endif //OFP_FLOWMODFIELD_H
