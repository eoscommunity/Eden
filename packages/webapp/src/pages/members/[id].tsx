import { GetServerSideProps } from "next";
import { QueryClient, useQuery } from "react-query";
import { dehydrate } from "react-query/hydration";

import {
    CallToAction,
    Container,
    FluidLayout,
    LoadingCard,
    queryMemberData,
    SingleColLayout,
} from "_app";
import { MemberCard, MemberCollections, MemberHoloCard } from "members";
import { ROUTES } from "_app/config";

/**
 * We have an issue if the member is not found in the development environment
 * due to dehydration with JSON not being able to serialize `undefined`:
 * Error: Error serializing `.dehydratedState.queries[0].state.data`
 * returned from `getServerSideProps` in "/members/[id]".
 * Reason: `undefined` cannot be serialized as JSON. Please use `null`
 * or omit this value.
 * Let's track this here: https://github.com/tannerlinsley/react-query/issues/1978
 */
export const getServerSideProps: GetServerSideProps = async ({ params }) => {
    const account = params!.id as string;

    const queryClient = new QueryClient();
    await queryClient.prefetchQuery(queryMemberData(account));

    return { props: { account, dehydratedState: dehydrate(queryClient) } };
};

interface Props {
    account: string;
}

export const MemberPage = ({ account }: Props) => {
    const { data: member, isLoading } = useQuery({
        ...queryMemberData(account),
        keepPreviousData: true,
    });

    if (member) {
        return (
            <FluidLayout title={`${member.name}'s Profile`}>
                <Container className="space-y-2.5 border-b">
                    <div className="flex items-center space-y-10 xl:space-y-0 xl:space-x-4 flex-col xl:flex-row">
                        <div className="max-w-2xl">
                            <MemberHoloCard member={member} />
                        </div>
                        <MemberCard member={member} />
                    </div>
                </Container>
                <MemberCollections member={member} />
            </FluidLayout>
        );
    }

    if (isLoading) {
        return (
            <SingleColLayout title="Loading member details...">
                <LoadingCard />
            </SingleColLayout>
        );
    }

    return (
        <SingleColLayout title="Member not found">
            <CallToAction
                href={ROUTES.MEMBERS.href}
                buttonLabel="Browse members"
            >
                This account is not an active Eden member.
            </CallToAction>
        </SingleColLayout>
    );
};

export default MemberPage;
