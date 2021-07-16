import React from "react";
import { GetServerSideProps } from "next";
import { QueryClient, useQuery } from "react-query";
import { dehydrate } from "react-query/hydration";

import { queryMembers, queryMemberStats, RawLayout, Text } from "_app";

export const getServerSideProps: GetServerSideProps = async ({ query }) => {
    const queryClient = new QueryClient();

    return {
        props: {
            dehydratedState: dehydrate(queryClient),
        },
    };
};

interface Props {
    delegatesPage: number;
}

const MEMBERS_PAGE_SIZE = 18;

export const DelegatesPage = (props: Props) => {
    const { isError: isMembersDataFetchError, data: members } = useQuery({
        ...queryMembers(1, MEMBERS_PAGE_SIZE),
        keepPreviousData: true,
    });

    const {
        isError: isMemberStatsDataFetchError,
        data: memberStats,
    } = useQuery({
        ...queryMemberStats,
        keepPreviousData: true,
    });

    return (
        <RawLayout title="Election">
            <Text size="sm" className="mb-8">
                Note: Data is in square brackets if it's not JSON (to show if
                something's undefined)
            </Text>
            <Text size="lg" className="bg-gray-200">
                Members
            </Text>
            <div className="grid grid-cols-2">
                <div>
                    <Text size="lg" className="mb-4">
                        -- Raw Table Data --
                    </Text>
                    <Text size="sm" className="mb-4">
                        <code>{`enum for status { pending = 0, active }`}</code>
                    </Text>
                    <Text size="sm" className="mb-4">
                        <code>
                            {`enum for election_participation_status { no_donation = 0, in_election, not_in_election, recently_inducted }`}
                        </code>
                    </Text>
                    <div>
                        <Text size="sm">
                            Sampling a single member for space...
                        </Text>
                        <pre>
                            {JSON.stringify(
                                (members && members.length && members[0]) || {},
                                null,
                                2
                            )}
                        </pre>
                    </div>
                </div>
            </div>
            <Text size="lg" className="bg-gray-200">
                Member Stats
            </Text>
            <div className="grid grid-cols-2">
                <div>
                    <Text size="lg" className="mb-4">
                        -- Raw Table Data --
                    </Text>
                    <Text size="sm" className="mb-4">
                        Note: the new field `ranks[]` is{" "}
                        <span className="font-bold">
                            not relevant to the frontend
                        </span>
                        ; it's a convenience for the smart contract. It's the
                        number of people at each rank, ranks[ranks.length-1]
                        being 1 for the Head Chief, ranks[ranks.length-2] being
                        the number of Chiefs, etc.
                    </Text>
                    <div>
                        <pre>{JSON.stringify(memberStats || {}, null, 2)}</pre>
                    </div>
                </div>
            </div>
        </RawLayout>
    );
};

export default DelegatesPage;
