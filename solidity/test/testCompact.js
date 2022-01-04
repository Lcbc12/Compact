const Compact = artifacts.require("Compact");

contract('Compact', function(accounts) {
    client = accounts[0];
    server = accounts[1];

    // estimated gas cost: 175522
    it("should send a challenge", async function() {
        let contract = await Compact.deployed();
        await contract.challenge([1, 1], [467, 491], {from:client});
        let challenge_nu = 
            (
                await contract.getChallenge(
                    await contract.getChallengesLength()
                )
            ).nu[0];
        await assert.equal(challenge_nu, 467, "Challenge not sent");
    });

    // estimated gas cost: 313922
    it("should verify the proof", async function() {
        let contract = await Compact.deployed();
        await contract.proof(
            [103464, 92926],
            web3.utils.toBN("19008947739600984228044157580001253402267228866197661222896813637769210210538"),
            web3.utils.toBN("12063506687536463007474726253078490541120193933064005433064625988901485573508"),
            {from:server}
        );
        let result = 
            (
                await contract.getProof(
                    await contract.getProofsLength()
                )
            ).valid;
        await assert.equal(result, true, "Proof not valid");
    });
});