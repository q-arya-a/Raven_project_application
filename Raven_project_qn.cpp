#include<iostream>
#include<cstdint>
#include<unordered_map>
#include<unordered_set>
#include<memory>
#include<sstream>
#include<vector>
#include<string>

//<--------------------------------------START OF PUBSUB INTERFACE--------------------------------------------------------------->

// publisher interface
class Publisher{
    public:
        // Data struct that contains generic info like instrument ID and last Traded price
        struct Data{
            std::uint64_t instrumentId_;
            double lastTradedPrice_;

            //constructor
            Data(std::uint64_t id, double price) : instrumentId_(id), lastTradedPrice_(price) {}
        };


        //pure virtual setter
        virtual void update_data(double price, double extraData) = 0;

        //pure virtual subscribe
        virtual void subscribe(char subscriberType, std::uint64_t subscriberID) = 0;

        //pure virtual get_data method
        virtual void get_data(char subscriberType, std::uint64_t subscriberId) = 0;
};

// Equity Publisher Class
class EquityPub : public Publisher{
    public:
        std::unordered_set<std::uint64_t> PaidSubscriberID;     // set that contains the ID's of paid subscribers to this publisher
        std::unordered_map<std::uint64_t, std::uint64_t> FreeSubscriberID;   // maps free subscriber ID's to number of attempts used

        // derived struct containing class-specific data (last day volume)
        struct EquityData : Data{
            std::uint64_t lastDayVolume_;

            //constructor
            EquityData(std::uint64_t id, double price, std::uint64_t vol) : Data(id, price), lastDayVolume_(vol) {}
        };
        EquityData equityData;

        //constructor
        EquityPub(std::uint64_t id, double price, std::uint64_t vol) : equityData(id, price, vol) {} 

        //setter (update values)
        void update_data(double price, double extraData) override {
            equityData.lastTradedPrice_ = price;
            equityData.lastDayVolume_ = static_cast<std::uint64_t>(extraData);
        }

        //getter (get values)
        // directly prints the data if valid or prints "invalid request"
        void get_data(char subscriberType, std::uint64_t subscriberId) override {
            if (subscriberType == 'P'){
                if (PaidSubscriberID.find(subscriberId) != PaidSubscriberID.end()){
                    std::cout<<subscriberType<<", "<<subscriberId<<", "<<equityData.instrumentId_<<", "<<equityData.lastTradedPrice_<<", "<<equityData.lastDayVolume_<<"\n";
                }
                else{
                    std::cout<<subscriberType<<", "<<subscriberId<<", "<<equityData.instrumentId_<<", "<<"invalid_request\n";
                }
            }
            else if (subscriberType == 'F'){
                if (FreeSubscriberID.find(subscriberId) != FreeSubscriberID.end() && FreeSubscriberID[subscriberId] < 100){
                    std::cout<<subscriberType<<", "<<subscriberId<<", "<<equityData.instrumentId_<<", "<<equityData.lastTradedPrice_<<", "<<equityData.lastDayVolume_<<"\n";
                    FreeSubscriberID[subscriberId]++;
                }
                else{
                   std::cout<<subscriberType<<", "<<subscriberId<<", "<<equityData.instrumentId_<<", "<<"invalid_request\n";
                }
            }

        }

        //subscribe
        void subscribe(char subscriberType, std::uint64_t subscriberID) override {
            if (subscriberType == 'P'){
                PaidSubscriberID.insert(subscriberID);     // adds the ID to the set PaidSubscriberID
            }
            else if (subscriberType == 'F'){
                FreeSubscriberID.insert({subscriberID, 0});    // adds the ID to the map FreeSubscriberID with a value 0 as 
                                                               // it has not used any attempts so far
            } 
        }
};

// Bond publisher class
class BondPub : public Publisher{
    public: 
        std::unordered_set<std::uint64_t> PaidSubscriberID;     // set that contains the ID's of paid subscribers to this publisher
        std::unordered_map<std::uint64_t, std::uint64_t> FreeSubscriberID;  // maps free subscriber ID's to number of attempts used

        // derived struct that contains class-specific data (bondyield)
        struct BondData : Data{
            double bondYield_;

            //constructor
            BondData(std::uint64_t id, double price, double bond) : Data(id, price), bondYield_(bond) {}
        };
        BondData bondData;
    
        //constructor
        BondPub(std::uint64_t id, double price, double bond) : bondData(id, price, bond) {}

        //setter (update values)
        void update_data(double price, double extraData) override {
            bondData.lastTradedPrice_ = price;
            bondData.bondYield_ = extraData;
        }

        //getter (get values)
        // directly prints the data if valid or prints "invalid request"
        void get_data(char subscriberType, std::uint64_t subscriberId) override {
            if (subscriberType == 'P'){
                if (PaidSubscriberID.find(subscriberId) != PaidSubscriberID.end()){
                    std::cout<<subscriberType<<", "<<subscriberId<<", "<<bondData.instrumentId_<<", "<<bondData.lastTradedPrice_<<", "<<bondData.bondYield_<<"\n";
                }
                else{
                    std::cout<<subscriberType<<", "<<subscriberId<<", "<<bondData.instrumentId_<<", "<<"invalid_request\n";
                }
            }
            else if (subscriberType == 'F'){
                if (FreeSubscriberID.find(subscriberId) != FreeSubscriberID.end() && FreeSubscriberID[subscriberId] < 100){
                    std::cout<<subscriberType<<", "<<subscriberId<<", "<<bondData.instrumentId_<<", "<<bondData.lastTradedPrice_<<", "<<bondData.bondYield_<<"\n";
                    FreeSubscriberID[subscriberId]++;
                }
                else{
                   std::cout<<subscriberType<<", "<<subscriberId<<", "<<bondData.instrumentId_<<", "<<"invalid_request\n";
                }
            }

        }

        //subscribe
        void subscribe(char subscriberType, std::uint64_t subscriberID) override {
            if (subscriberType == 'P'){
                PaidSubscriberID.insert(subscriberID);      // adds the ID to the set PaidSubscriberID
            }
            else if (subscriberType == 'F'){
                FreeSubscriberID.insert({subscriberID, 0});     // adds the ID to the map FreeSubscriberID with a value 0 as 
                                                               // it has not used any attempts so far
            } 
        }
};

// Store publishers using ID in a map
std::unordered_map<std::uint64_t, std::unique_ptr<Publisher>> publisherMap;

// Subscriber Interface
class Subscriber{
    public:

        // calls the subscribe method of the publisher which subscribes the subscriber ID to the corresponding publisher
        void subscribe(std::uint64_t instrumentId, char subscriberType, std::uint64_t subscriberId){
            auto it = publisherMap.find(instrumentId);
            if(it != publisherMap.end()){
                it->second->subscribe(subscriberType, subscriberId);
            }
        }

        // calls the get_data method of the corresponding publisher if it exists, else just prints invalid request
        void get_data(std::uint64_t instrumentId, char subscriberType, std::uint64_t subscriberId){
            auto it = publisherMap.find(instrumentId);
            if(it != publisherMap.end()){
                it->second->get_data(subscriberType, subscriberId);
            }

            else{
                std::cout<<subscriberType<<", "<<subscriberId<<", "<<instrumentId<<", "<<"invalid_request\n";
            }
        }

};

// Free subscriber derived class
class FreeSub : public Subscriber{
    public:
        FreeSub() {}
};

// Paid subscriber derived class
class PaidSub : public Subscriber{
    public:
        PaidSub() {}
};

// Store subscribers using ID in a map
std::unordered_map<std::uint64_t, std::unique_ptr<Subscriber>> subscriberMap;


// external helper function for publisher interface (to be used by the user)
void publisher(std::uint64_t instrumentId, double price, double extraData)
/* This function updates the data of the publisher if it exists, else, creates a new publisher object with the specified values 
*/
{
    auto it = publisherMap.find(instrumentId);
    if(it != publisherMap.end()){
        it->second->update_data(price, extraData);
    }

    else{
        if (instrumentId >= 1000) {
            //Create Bond Publisher
            publisherMap[instrumentId] = std::make_unique<BondPub>(instrumentId, price, extraData);
        } 
        else {
            //Create Equity Publisher
            publisherMap[instrumentId] = std::make_unique<EquityPub>(instrumentId, price, static_cast<std::uint64_t>(extraData));
        }
    }
}


// external helper function for subscriber interface (to be used by the user)
void subscriber(std::uint64_t instrumentId, char subscriberType, char operation, std::uint64_t subscriberId)
/* This function either subscribes or retrieves data based on the operation type
*/
{

    //subscribe operation
    if(operation == 'S')
    /* If the subscriber object already exists, then just subscribe to the corresponding publisher, else create a new subscriber object
        and then subscribe
    */
    {
        auto it = subscriberMap.find(subscriberId);
        if(it == subscriberMap.end()){
            if(subscriberType == 'P'){
                subscriberMap[subscriberId] = std::make_unique<PaidSub>();
            }
            else if(subscriberType == 'F'){
                subscriberMap[subscriberId] = std::make_unique<FreeSub>();
            }
            subscriberMap[subscriberId]->subscribe(instrumentId, subscriberType, subscriberId);
        }
        else{
            it->second->subscribe(instrumentId, subscriberType, subscriberId);
        }
    }

    //get-data operation
    else if(operation == 'G')
    /* Get the data if the instrument exists, else print "invalid request"
    */
    {
            auto it = subscriberMap.find(subscriberId);
            if(it != subscriberMap.end()){
                it->second->get_data(instrumentId, subscriberType, subscriberId);
            }

            else{
                std::cout<<subscriberType<<", "<<subscriberId<<", "<<instrumentId<<", "<<"invalid_request\n";
            }

    }
}
    //  <-----------------------------------------------END OF PUBSUB INTERFACE------------------------------------------------->



//input parser function for parsing the input strings 
void parse_input(const std::string& inputLine) {
    std::istringstream iss(inputLine);
    std::vector<std::string> tokens;
    std::string token;

    //tokenize the input line
    while (iss >> token) {
        tokens.push_back(token);
    }

    if(tokens[0] == "P"){
        std::uint64_t instrumentId = std::stoi(tokens[1]);
        double price = std::stod(tokens[2]);
        double data = std::stod(tokens[3]);

        publisher(instrumentId, price, data);
    }
    else if(tokens[0] == "S"){
            char subscriberType = tokens[1][0];
            std::uint64_t subscriberID = std::stoi(tokens[2]);
            char operation = std::toupper(tokens[3][0]);
            std::uint64_t instrumentId = std::stoi(tokens[4]);
            subscriber(instrumentId, subscriberType, operation, subscriberID);
    }

}


int main(){
    parse_input("P 1383 846930886 27.770000");
    parse_input("S F 1957747793 subscribe 1383");
    for(int i=0;i<=100;i++)
    parse_input("S F 1957747793 get_data 1383");

}


