pragma solidity >=0.4.24 <=0.7.0;

contract AcceptResult{
    address DU;
    address FPC;
   
   //字典 key映射到value 
    mapping(bytes32 => uint[]) private RSTList;
    event getResultEvent(bytes32 token);

    
    function receiveRST(bytes32 token, uint[] memory resultids) public {
        require(msg.sender == FPC, "Only FPC");
        RSTList[token] = resultids;
        emit getResultEvent(token);
    }
    
    function getRST() public view returns(uint[] memory resultids){
        return result;
    }
}