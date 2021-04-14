import { UALProvider, withUAL } from "ual-reactjs-renderer";

import { ClientOnly, RawLayout } from "_app";
import { appName, chainConfig } from "config";
import { anchor, CreatorForm } from "nfts";

const CreatorUalForm = withUAL(CreatorForm);

export const MembersCreatorPage = () => {
    return (
        <RawLayout title="NFT Creator">
            <ClientOnly>
                <UALProvider
                    chains={[chainConfig]}
                    authenticators={[anchor]}
                    appName={appName}
                >
                    <CreatorUalForm />
                </UALProvider>
            </ClientOnly>
        </RawLayout>
    );
};

export default MembersCreatorPage;
