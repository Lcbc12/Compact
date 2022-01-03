const Compact = artifacts.require("Compact");

contract('Compact', function(accounts) {
    client = accounts[0];
    server = accounts[1];

    it("should send a challenge", async function() {
        let contract = await Compact.deployed();
        await contract.challenge(2, 418, {from:client});
        let challenge_nu = 
            (
                await contract.index_challenges(
                    await contract.getChallengesLength()
                )
            ).nu;
        await assert.equal(418, challenge_nu, "Challenge not sent");
    });

    it("should verify the proof", async function() {
        let contract = await Compact.deployed();
        await contract.proof(
            45144,
            web3.utils.toBN("18406874792596278302199111963446125795395919970852221058561807340620231197048"),
            web3.utils.toBN("14613361525272065441192946791976026129725287577577546941926976800957822849950"),
            {from:server}
        );
        let result = 
            (
                await contract.index_proofs(
                    await contract.getProofsLength()
                )
            ).valid;
        await assert.equal(true, result, "Proof not valid");
    });
});