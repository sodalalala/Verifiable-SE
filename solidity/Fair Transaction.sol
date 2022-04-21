pragma solidity >=0.4.24 <=0.7.0;

import "./ResultReceiving.sol";
import "./ResultVerification.sol";
import "./Functions.sol";
pragma experimental ABIEncoderV2;

contract FairPayment {
    // 定义DU的结构
    struct dataUser{
        bytes32 token;
        uint256 balance;
        address UIC; // 自己的接口地址 
    }
    address payable private DO;
    address payable private CP;
    mapping(address => dataUser) private addrToDU;
    uint256 constant private fee = 2;
    bool results;
    modifier onlyRelated {
        require(msg.sender == DO || msg.sender == CP , "only Related");
        _;
    }                                                                                            
    event ResultEvent(address payable _user, bool _f);

    
    
    //DU 调用， 进行存款 
    function BalanceValue() payable public returns(uint256) {
        if(msg.value != 0){
            addrToDU[msg.sender].balance += msg.value;
        }
        return addrToDU[msg.sender].balance;
    }
    //DU 调用,对余额进行判断，若余额充足，传递token，申请进行搜索操作 
    function Qualification(bytes32 _token, address _UIC) public {
        assert(addrToDU[msg.sender].balance >= fee);
        addrToDU[msg.sender].token = _token;
        addrToDU[msg.sender].UIC = _UIC;
    }
    
    function FeeTrans(address payable _user, ResultVerification RS ,Functions F,uint [] memory resultids) public {
        results = RS.Verify(F,resultids);
        if(results) {
            DO.transfer(fee/2);
            CP.transfer(fee/2);
            addrToDU[_user].balance -= fee;
            AcceptResult(addrToDU[_user].UIC).receiveRST(addrToDU[_user].token, resultids);
        } else {
            _user.transfer(fee);
            addrToDU[_user].balance -= fee;
        }
        emit ResultEvent(_user, results);
    }
}