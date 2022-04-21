import "truffle/Assert.sol";
import "truffle/DeployedAddresses.sol";
import "../contracts/ResultVerify.sol";


contract TestRV{
    function testSetup() public {
        ResultVerify rv = ResultVerify(DeployedAddresses.ResultVerify());
        bool result = rv.Verify();
        Assert.equal(true, result, "The results are correct and completely");
        
    }
}
