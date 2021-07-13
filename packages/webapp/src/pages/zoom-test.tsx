import { useEffect } from "react";
import { GetServerSideProps } from "next";

import {
    zoomRequestAuth,
    zoomConnectAccountLink,
    zoomAccountJWTIsExpired,
    zoomResponseIsInvalidAccess,
} from "_api/zoom-commons";
import {
    SingleColLayout,
    CallToAction,
    useUALAccount,
    Button,
    useZoomAccountJWT,
} from "_app";

export const getServerSideProps: GetServerSideProps = async ({ query }) => {
    const oauthCode = (query.code as string) || "";
    let newZoomAccountJWT = null;

    if (oauthCode) {
        const oauthDataResponse = await zoomRequestAuth(oauthCode);
        if (oauthDataResponse.error) {
            console.error(
                "fail to auth zoom code",
                oauthCode,
                oauthDataResponse
            );
        } else {
            newZoomAccountJWT = oauthDataResponse;
        }
    }

    return {
        props: {
            newZoomAccountJWT,
        },
    };
};

interface Props {
    newZoomAccountJWT: any;
}

export const ZoomTestPage = ({ newZoomAccountJWT }: Props) => {
    const [ualAccount, _, ualShowModal] = useUALAccount();
    const [_zoomAccountJWT, setZoomAccountJWT] = useZoomAccountJWT(null);

    useEffect(() => {
        if (newZoomAccountJWT) {
            setZoomAccountJWT(newZoomAccountJWT);
            console.info("new zoom credentials set", newZoomAccountJWT);
        }
    }, [newZoomAccountJWT]);

    return (
        <SingleColLayout title="Zoom Test">
            {ualAccount ? (
                <ZoomTestContainer ualAccount={ualAccount} />
            ) : (
                <CallToAction buttonLabel="Sign in" onClick={ualShowModal}>
                    Welcome to Eden. Sign in using your wallet.
                </CallToAction>
            )}
        </SingleColLayout>
    );
};

export default ZoomTestPage;

const ZoomTestContainer = ({ ualAccount }: any) => {
    const [zoomAccountJWT, setZoomAccountJWT] = useZoomAccountJWT(null);

    const resetZoomAccount = () => {
        setZoomAccountJWT(null);
    };

    const generateZoomMeetingLink = async () => {
        let accessToken = zoomAccountJWT.access_token;
        if (!accessToken) {
            return alert("Invalid AccessToken");
        }

        if (zoomAccountJWTIsExpired(zoomAccountJWT)) {
            const newTokensResponse = await fetch(`/api/refresh-zoom`, {
                method: "POST",
                body: JSON.stringify({
                    refreshToken: zoomAccountJWT.refresh_token,
                }),
            });

            const newTokens = await newTokensResponse.json();
            console.info(newTokens);
            if (!newTokensResponse.ok) {
                resetZoomAccount();
            }

            setZoomAccountJWT(newTokens);
            accessToken = newTokens.access_token;
        }

        const response = await fetch(`/api/meeting-links`, {
            method: "POST",
            body: JSON.stringify({ accessToken, meetingClient: "zoom" }),
        });

        const responseData = await response.json();
        if (zoomResponseIsInvalidAccess(responseData)) {
            resetZoomAccount();
        }

        console.info(responseData);
        alert(JSON.stringify(responseData, undefined, 2));
    };

    console.info(zoomAccountJWT);

    return (
        <div>
            {zoomAccountJWT ? (
                <div>
                    Thanks for connecting Eden to your Zoom account.
                    <p>
                        Next step is to create a meeting. <br />
                        <Button onClick={generateZoomMeetingLink}>
                            Create a new Meeting
                        </Button>
                    </p>
                </div>
            ) : (
                <Button href={zoomConnectAccountLink}>
                    Link your Zoom Account
                </Button>
            )}
        </div>
    );
};
